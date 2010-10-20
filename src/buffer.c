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
	buffer->position = 0;

	return buffer;
}

void buffer_clear(buffer_t* buffer) {
	buffer->position = 0;
	buffer->data[buffer->position] = 0;
}

void buffer_delete(buffer_t* buffer) {
	pool_free(buffer->pool, buffer->data);
	pool_free(buffer->pool, buffer);
}

int buffer_grow(buffer_t* buffer) {
	buffer->data = (uint8_t*)pool_reallocate(buffer->pool, buffer->data, buffer->length + BUFFER_SIZE);
	
	buffer->length += BUFFER_SIZE;

	memset(buffer->data + buffer->position, 0, buffer->length - buffer->position);

	return buffer->length - buffer->position;
}

void buffer_set_position(buffer_t* buffer, uint32_t position) {
	while(buffer->length < position) {
		buffer_grow(buffer);
	}

	buffer->position = position;
}

void buffer_writen(buffer_t* buffer, const uint8_t* value, size_t length) {
	while((buffer->length - buffer->position) < length + 1) {
		buffer_grow(buffer);
	}

	memcpy(buffer->data + buffer->position, value, length);
	
	buffer->position += length;
}

void buffer_write(buffer_t* buffer, uint8_t byte) {
	if((buffer->length - buffer->position) < 2) {
		buffer_grow(buffer);
	}

	buffer->data[buffer->position++] = byte;
}

void buffer_write_int32(buffer_t* buffer, int32_t value) {
	int32_u u;

	u.value = value;

	for(int i = 0; i < 4; i ++) {
		buffer_write(buffer, u.bytes[i]);
	}
}

uint8_t buffer_read_uint8(buffer_t* buffer) {
	if((buffer->length - buffer->position) > 1) {
		return buffer->data[buffer->position++];
	}

	// throw exception
}

uint16_t buffer_read_uint16(buffer_t* buffer) {
	if((buffer->length - buffer->position) > 2) {
		uint16_u u;

		for(int i = 0; i < 2; i ++) {
			u.bytes[i] = buffer->data[buffer->position++];
		}

		return u.value;
	}

	// throw exception
}

