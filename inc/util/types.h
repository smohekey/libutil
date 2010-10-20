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

#ifndef _UTIL_TYPES_H
#define _UTIL_TYPES_H

#include <stdint.h>

typedef union _int16_u {
	int8_t bytes[2];
	int16_t value;
} int16_u;

typedef union _int32_u {
	int8_t bytes[4];
	int32_t value;
} int32_u;

typedef union _int64_u {
	int8_t bytes[8];
	int64_t value;
} int64_u;

typedef union _uint16_u {
	int8_t bytes[2];
	uint16_t value;
} uint16_u;

typedef union _uint32_u {
	int8_t bytes[4];
	uint32_t value;
} uint32_u;

typedef union _uint64_u {
	int8_t bytes[8];
	uint64_t value;
} uint64_u;

typedef union _double_u {
	int8_t bytes[8];
	double value;
} double_u;

#endif

