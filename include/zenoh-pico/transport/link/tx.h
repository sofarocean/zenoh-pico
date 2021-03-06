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

#ifndef ZENOH_PICO_TRANSPORT_LINK_TX_H
#define ZENOH_PICO_TRANSPORT_LINK_TX_H

#include "zenoh-pico/api/session.h"
#include "zenoh-pico/transport/transport.h"
#include "zenoh-pico/link/link.h"

void __unsafe_zn_prepare_wbuf(_z_wbuf_t *buf, int is_streamed);
void __unsafe_zn_finalize_wbuf(_z_wbuf_t *buf, int is_streamed);
_zn_transport_message_t __zn_frame_header(zn_reliability_t reliability, int is_fragment, int is_final, z_zint_t sn);
int __unsafe_zn_serialize_zenoh_fragment(_z_wbuf_t *dst, _z_wbuf_t *src, zn_reliability_t reliability, size_t sn);

/*------------------ Transmission and Reception helpers ------------------*/
int _zn_unicast_send_z_msg(zn_session_t *zn, _zn_zenoh_message_t *z_msg, zn_reliability_t reliability, zn_congestion_control_t cong_ctrl);
int _zn_multicast_send_z_msg(zn_session_t *zn, _zn_zenoh_message_t *z_msg, zn_reliability_t reliability, zn_congestion_control_t cong_ctrl);

int _zn_send_t_msg(_zn_transport_t *zt, const _zn_transport_message_t *t_msg);
int _zn_unicast_send_t_msg(_zn_transport_unicast_t *ztu, const _zn_transport_message_t *t_msg);
int _zn_multicast_send_t_msg(_zn_transport_multicast_t *ztm, const _zn_transport_message_t *t_msg);

int _zn_link_send_t_msg(const _zn_link_t *zl, const _zn_transport_message_t *t_msg);

#endif /* ZENOH_PICO_TRANSPORT_LINK_TX_H */