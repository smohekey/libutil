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

#ifndef _UTIL_TOKENISER_H
#define _UTIL_TOKENISER_H

#include <stdlib.h>
#include <stdint.h>

#include <util/pool.h>

#define TOKENISER_BINARY (1<<0)
#define TOKENISER_WHITESPACE (1<<1)

typedef struct _tokeniser_s {
	uint8_t flags;
	const char* buffer;
	uint64_t index;
	uint64_t length;
} tokeniser_t;

extern tokeniser_t* tokeniser_create(pool_t* pool, uint8_t flags, const char* buffer);
extern tokeniser_t* tokeniser_createn(pool_t* pool, uint8_t flags, const char* buffer, size_t length);
extern void tokeniser_init(tokeniser_t* tokeniser, uint8_t flags, const char* buffer);
extern void tokeniser_initn(tokeniser_t* tokeniser, uint8_t flags, const char* buffer, size_t length);
extern int8_t tokeniser_next(tokeniser_t* tokeniser);
extern void tokeniser_enable_flags(tokeniser_t* tokeniser, uint8_t flags);
extern void tokeniser_disable_flags(tokeniser_t* tokeniser, uint8_t flags);
extern int8_t tokeniser_current(tokeniser_t* tokeniser);
extern int8_t tokeniser_peek(tokeniser_t* tokeniser);
extern void tokeniser_expect(tokeniser_t* tokeniser, char expected);
extern void tokeniser_copy(tokeniser_t* tokeniser, void* dest, size_t length);

#endif /*_UTIL_TOKENISER_H*/
