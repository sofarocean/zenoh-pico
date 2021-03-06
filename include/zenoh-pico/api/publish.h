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

#ifndef ZENOH_PICO_PUBLISH_API_H
#define ZENOH_PICO_PUBLISH_API_H

#include "zenoh-pico/protocol/core.h"

/**
 * Return type when declaring a publisher.
 */
typedef struct
{
    void *zn; // FIXME: zn_session_t *zn;
    z_zint_t id;
    zn_reskey_t key;
} zn_publisher_t;

#endif /* ZENOH_PICO_PUBLISH_API_H */
