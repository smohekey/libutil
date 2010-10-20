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

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <util/buffer.h>
#include <util/types.h>

#define BUFFER_SIZE 128

buffer_t* buffer_create(pool_t* pool) {
	buffer_t* buffer = (buffer_t*)pool_allocate(pool, sizeof(buffer_t));
	
	buffer->pool = pool;
	buffer->data = (uint8_t*)pool_allocate(buffer->pool, BUFFER_SIZE);
	memset(buffer->data, 0, BUFFER_SIZE);
	buffer->length = BUFFER_SIZE;
	buffer->index = 0;

	return buffer;
}

void buffer_clear(buffer_t* buffer) {
	buffer->index = 0;
	buffer->data[buffer->index] = 0;
}

void buffer_delete(buffer_t* buffer) {
	pool_free(buffer->pool, buffer->data);
	pool_free(buffer->pool, buffer);
}

int buffer_grow(buffer_t* buffer) {
	buffer->data = (uint8_t*)pool_reallocate(buffer->pool, buffer->data, buffer->length + BUFFER_SIZE);
	
	buffer->length += BUFFER_SIZE;

	memset(buffer->data + buffer->index, 0, buffer->length - buffer->index);

	return buffer->length - buffer->index;
}

void buffer_set_position(buffer_t* buffer, uint32_t position) {
	while(buffer->length < position) {
		buffer_grow(buffer);
	}

	buffer->index = position;
}

void buffer_appendn(buffer_t* buffer, const uint8_t* value, size_t length) {
	while((buffer->length - buffer->index) < length + 1) {
		buffer_grow(buffer);
	}

	memcpy(buffer->data + buffer->index, value, length);
	
	buffer->index += length;
}

void buffer_append(buffer_t* buffer, uint8_t byte) {
	if((buffer->length - buffer->index) < 2) {
		buffer_grow(buffer);
	}

	buffer->data[buffer->index++] = byte;
}

void buffer_append_int32(buffer_t* buffer, int32_t value) {
	int32_u u;

	u.value = value;

	for(int i = 0; i < 4; i ++) {
		buffer_append(buffer, u.bytes[i]);
	}
}

