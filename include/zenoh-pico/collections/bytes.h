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

#ifndef ZENOH_PICO_COLLECTIONS_BYTES_H
#define ZENOH_PICO_COLLECTIONS_BYTES_H

#include <stdint.h>
#include <stdio.h>

/*-------- Bytes --------*/
/**
 * An array of bytes.
 *
 * Members:
 *   size_t len: The length of the bytes array.
 *   uint8_t *val: A pointer to the bytes array.
 */
typedef struct z_bytes_t
{
    const uint8_t *val;
    size_t len;
    uint8_t is_alloc;
} z_bytes_t;

void _z_bytes_init(z_bytes_t *bs, size_t capacity);
z_bytes_t _z_bytes_make(size_t capacity);
z_bytes_t _z_bytes_wrap(const uint8_t *bs, size_t len);

void _z_bytes_copy(z_bytes_t *dst, const z_bytes_t *src);
z_bytes_t _z_bytes_duplicate(const z_bytes_t *src);
void _z_bytes_move(z_bytes_t *dst, z_bytes_t *src);
void _z_bytes_reset(z_bytes_t *bs);
int _z_bytes_is_empty(const z_bytes_t *bs);

int _z_bytes_eq(const z_bytes_t *left, const z_bytes_t *right);
void _z_bytes_clear(z_bytes_t *bs);
void _z_bytes_free(z_bytes_t **bs);

#endif /* ZENOH_PICO_COLLECTIONS_BYTES_H */
