/*
 * Copyright 2010 Scott Mohekey
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _UTIL_BUFFER_H
#define _UTIL_BUFFER_H

#include <stdio.h>
#include <stdint.h>

#include <util/pool.h>

typedef struct _buffer_s {
	pool_t* pool;
	uint8_t* data;
	size_t length;
	size_t index;
} buffer_t;

extern buffer_t* buffer_create(pool_t* pool);
extern void buffer_delete(buffer_t* buffer);
extern void buffer_clear(buffer_t* buffer);
extern int buffer_grow(buffer_t* buffer);
extern void buffer_append(buffer_t* buffer, uint8_t value);
extern void buffer_appendn(buffer_t* buffer, const uint8_t* value, size_t length);
extern void buffer_append_int32(buffer_t* buffer, int32_t value);
extern void buffer_set_position(buffer_t* buffer, uint32_t position);

#endif
