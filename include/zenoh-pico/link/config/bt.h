/*
 * Copyright (c) 2017, 2021 ADLINK Technology Inc.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
 * which is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *
 * Contributors:
 *   ADLINK zenoh team, <zenoh@adlink-labs.tech>
 */

#ifndef ZENOH_PICO_LINK_CONFIG_BT_H
#define ZENOH_PICO_LINK_CONFIG_BT_H

#include "zenoh-pico/config.h"
#include "zenoh-pico/collections/intmap.h"
#include "zenoh-pico/collections/string.h"
#include "zenoh-pico/system/platform.h"

#if ZN_LINK_BLUETOOTH == 1

#define BT_CONFIG_MODE_KEY     0x01
#define BT_CONFIG_MODE_STR     "mode"

#define BT_CONFIG_PROFILE_KEY  0x02
#define BT_CONFIG_PROFILE_STR  "profile"

#define BT_CONFIG_TOUT_KEY     0x03
#define BT_CONFIG_TOUT_STR     "tout"

#define BT_CONFIG_MAPPING_BUILD          \
    int argc = 3;                        \
    _z_str_intmapping_t args[argc];      \
    args[0].key = BT_CONFIG_MODE_KEY;    \
    args[0].str = BT_CONFIG_MODE_STR;    \
    args[1].key = BT_CONFIG_PROFILE_KEY; \
    args[1].str = BT_CONFIG_PROFILE_STR; \
    args[2].key = BT_CONFIG_TOUT_KEY;    \
    args[2].str = BT_CONFIG_TOUT_STR;

size_t _zn_bt_config_strlen(const _z_str_intmap_t *s);

void _zn_bt_config_onto_str(z_str_t dst, const _z_str_intmap_t *s);
z_str_t _zn_bt_config_to_str(const _z_str_intmap_t *s);

_z_str_intmap_result_t _zn_bt_config_from_str(const z_str_t s);
_z_str_intmap_result_t _zn_bt_config_from_strn(const z_str_t s, size_t n);
#endif

#endif /* ZENOH_PICO_LINK_CONFIG_BT_H */