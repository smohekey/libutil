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

#ifndef _UTIL_STRING_H
#define _UTIL_STRING_H

#include <stdio.h>
#include <util/pool.h>

typedef struct _string_s {
	pool_t* pool;
	char* buffer;
	unsigned int length;
	unsigned int index;
} string_t;

extern string_t* string_create(pool_t* pool);
extern void string_delete(string_t* string);
extern void string_clear(string_t* string);
extern int string_grow(string_t* string);
extern void string_append(string_t* string, const char* value);
extern void string_appendn(string_t* string, const char* value, size_t length);
extern void string_append_char(string_t* string, char character);
extern void string_appendf(string_t* string, char* format, ...);

const char* strnchr(const char* s, size_t length, char c);

#endif
