//
// Copyright (c) 2022 ZettaScale Technology
//
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
// which is available at https://www.apache.org/licenses/LICENSE-2.0.
//
// SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
//
// Contributors:
//   ZettaScale Zenoh Team, <zenoh@zettascale.tech>
//

#include "zenoh-pico/session/utils.h"
#include "zenoh-pico/transport/link/rx.h"
#include "zenoh-pico/transport/utils.h"
#include "zenoh-pico/utils/logging.h"

/*------------------ Reception helper ------------------*/
void _zn_unicast_recv_t_msg_na(_zn_transport_unicast_t *ztu, _zn_transport_message_result_t *r)
{
    _Z_DEBUG(">> recv session msg\n");
    r->tag = _z_res_t_OK;

    // Acquire the lock
    z_mutex_lock(&ztu->mutex_rx);

    // Prepare the buffer
    _z_zbuf_reset(&ztu->zbuf);

    if (ztu->link->is_streamed == 1)
    {
        // Read the message length
        if (_zn_link_recv_exact_zbuf(ztu->link, &ztu->zbuf, _ZN_MSG_LEN_ENC_SIZE, NULL) != _ZN_MSG_LEN_ENC_SIZE)
        {
            r->tag = _z_res_t_ERR;
            r->value.error = _zn_err_t_IO_GENERIC;
            goto EXIT_SRCV_PROC;
        }

        size_t len = 0;
        for (int i = 0; i < _ZN_MSG_LEN_ENC_SIZE; i++)
            len |= _z_zbuf_read(&ztu->zbuf) << (i * 8);

        _Z_DEBUG(">> \t msg len = %hu\n", len);
        size_t writable = _z_zbuf_capacity(&ztu->zbuf) - _z_zbuf_len(&ztu->zbuf);
        if (writable < len)
        {
            r->tag = _z_res_t_ERR;
            r->value.error = _zn_err_t_IOBUF_NO_SPACE;
            goto EXIT_SRCV_PROC;
        }

        // Read enough bytes to decode the message
        if (_zn_link_recv_exact_zbuf(ztu->link, &ztu->zbuf, len, NULL) != len)
        {
            r->tag = _z_res_t_ERR;
            r->value.error = _zn_err_t_IO_GENERIC;
            goto EXIT_SRCV_PROC;
        }
    }
    else
    {
        if (_zn_link_recv_zbuf(ztu->link, &ztu->zbuf, NULL) == SIZE_MAX)
        {
            r->tag = _z_res_t_ERR;
            r->value.error = _zn_err_t_IO_GENERIC;
            goto EXIT_SRCV_PROC;
        }
    }

    // Mark the session that we have received data
    ztu->received = 1;

    _Z_DEBUG(">> \t transport_message_decode\n");
    _zn_transport_message_decode_na(&ztu->zbuf, r);

EXIT_SRCV_PROC:
    // Release the lock
    z_mutex_unlock(&ztu->mutex_rx);
}

_zn_transport_message_result_t _zn_unicast_recv_t_msg(_zn_transport_unicast_t *ztu)
{
    _zn_transport_message_result_t r;

    _zn_unicast_recv_t_msg_na(ztu, &r);
    return r;
}

int _zn_unicast_handle_transport_message(_zn_transport_unicast_t *ztu, _zn_transport_message_t *t_msg)
{
    switch (_ZN_MID(t_msg->header))
    {
    case _ZN_MID_SCOUT:
    {
        _Z_INFO("Handling of Scout messages not implemented\n");
        break;
    }

    case _ZN_MID_HELLO:
    {
        // Do nothing, zenoh-pico clients are not expected to handle hello messages
        break;
    }

    case _ZN_MID_INIT:
    {
        // Do nothing, zenoh clients are not expected to handle accept messages on established sessions
        break;
    }

    case _ZN_MID_OPEN:
    {
        // Do nothing, zenoh clients are not expected to handle accept messages on established sessions
        break;
    }

    case _ZN_MID_CLOSE:
    {
        _Z_INFO("Closing session as requested by the remote peer\n");
        break;
    }

    case _ZN_MID_SYNC:
    {
        _Z_INFO("Handling of Sync messages not implemented\n");
        break;
    }

    case _ZN_MID_ACK_NACK:
    {
        _Z_INFO("Handling of AckNack messages not implemented\n");
        break;
    }

    case _ZN_MID_KEEP_ALIVE:
    {
        _Z_INFO("Received ZN_KEEP_ALIVE message\n");
        break;
    }

    case _ZN_MID_PING_PONG:
    {
        _Z_INFO("Handling of PingPong messages not implemented\n");
        break;
    }

    case _ZN_MID_FRAME:
    {
        _Z_INFO("Received ZN_FRAME message\n");
        // Check if the SN is correct
        if (_ZN_HAS_FLAG(t_msg->header, _ZN_FLAG_T_R))
        {
            // @TODO: amend once reliability is in place. For the time being only
            //        monothonic SNs are ensured
            if (_zn_sn_precedes(ztu->sn_resolution_half, ztu->sn_rx_reliable, t_msg->body.frame.sn))
            {
                ztu->sn_rx_reliable = t_msg->body.frame.sn;
            }
            else
            {
                _z_wbuf_clear(&ztu->dbuf_reliable);
                _Z_INFO("Reliable message dropped because it is out of order\n");
                break;
            }
        }
        else
        {
            if (_zn_sn_precedes(ztu->sn_resolution_half, ztu->sn_rx_best_effort, t_msg->body.frame.sn))
            {
                ztu->sn_rx_best_effort = t_msg->body.frame.sn;
            }
            else
            {
                _z_wbuf_clear(&ztu->dbuf_best_effort);
                _Z_INFO("Best effort message dropped because it is out of order\n");
                break;
            }
        }

        if (_ZN_HAS_FLAG(t_msg->header, _ZN_FLAG_T_F))
        {
            // Select the right defragmentation buffer
            _z_wbuf_t *dbuf = _ZN_HAS_FLAG(t_msg->header, _ZN_FLAG_T_R) ? &ztu->dbuf_reliable : &ztu->dbuf_best_effort;

            uint8_t drop = 0;
            if (_z_wbuf_len(dbuf) + t_msg->body.frame.payload.fragment.len > ZN_FRAG_MAX_SIZE)
            {
                // Filling the wbuf capacity as a way to signling the last fragment to reset the dbuf
                // Otherwise, last (smaller) fragments can be understood as a complete message
                _z_wbuf_write_bytes(dbuf, t_msg->body.frame.payload.fragment.val, 0, _z_wbuf_space_left(dbuf));
                drop = 1;
            }
            else
            {
                // Add the fragment to the defragmentation buffer
                _z_wbuf_write_bytes(dbuf, t_msg->body.frame.payload.fragment.val, 0, t_msg->body.frame.payload.fragment.len);
            }

            // Check if this is the last fragment
            if (_ZN_HAS_FLAG(t_msg->header, _ZN_FLAG_T_E))
            {
                // Drop message if it is bigger the max buffer size
                if (drop == 1)
                {
                    _z_wbuf_reset(dbuf);
                    break;
                }

                // Convert the defragmentation buffer into a decoding buffer
                _z_zbuf_t zbf = _z_wbuf_to_zbuf(dbuf);

                // Decode the zenoh message
                _zn_zenoh_message_result_t r_zm = _zn_zenoh_message_decode(&zbf);
                if (r_zm.tag == _z_res_t_OK)
                {
                    _zn_zenoh_message_t d_zm = r_zm.value.zenoh_message;
                    _zn_handle_zenoh_message(ztu->session, &d_zm);

                    // Clear must be explicitly called for fragmented zenoh messages.
                    // Non-fragmented zenoh messages are released when their transport message is released.
                    _zn_z_msg_clear(&d_zm);
                }

                // Free the decoding buffer
                _z_zbuf_clear(&zbf);
                // Reset the defragmentation buffer
                _z_wbuf_reset(dbuf);
            }

            break;
        }
        else
        {
            // Handle all the zenoh message, one by one
            unsigned int len = _z_vec_len(&t_msg->body.frame.payload.messages);
            for (unsigned int i = 0; i < len; i++)
                _zn_handle_zenoh_message(ztu->session, (_zn_zenoh_message_t *)_z_vec_get(&t_msg->body.frame.payload.messages, i));
        }
        break;
    }

    default:
    {
        _Z_ERROR("Unknown session message ID\n");
        break;
    }
    }

    return _z_res_t_OK;
}
