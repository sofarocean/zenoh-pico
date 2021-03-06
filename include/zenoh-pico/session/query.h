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

#ifndef ZENOH_PICO_SESSION_QUERY_H
#define ZENOH_PICO_SESSION_QUERY_H

#include "zenoh-pico/api/session.h"

/*------------------ Query ------------------*/
z_zint_t _zn_get_query_id(zn_session_t *zn);

_zn_pending_query_t *_zn_get_pending_query_by_id(zn_session_t *zn, const z_zint_t id);

int _zn_register_pending_query(zn_session_t *zn, _zn_pending_query_t *pq);
int _zn_trigger_query_reply_partial(zn_session_t *zn, const _zn_reply_context_t *reply_context, const zn_reskey_t reskey, const z_bytes_t payload, const _zn_data_info_t data_info);
int _zn_trigger_query_reply_final(zn_session_t *zn, const _zn_reply_context_t *reply_context);
void _zn_unregister_pending_query(zn_session_t *zn, _zn_pending_query_t *pq);
void _zn_flush_pending_queries(zn_session_t *zn);

#endif /* ZENOH_PICO_SESSION_QUERY_H */
