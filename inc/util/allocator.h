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

#ifndef _UTIL_ALLOCATOR_H_
#define _UTIL_ALLOCATOR_H_

#include <stdint.h>

typedef void* (*allocator_allocate_f)( uint64_t size );
typedef void* (*allocator_reallocate_f)( void* allocation, uint64_t newSize );
typedef void (*allocator_free_f)( void* allocation );
	
typedef struct _allocator {
	allocator_allocate_f allocate;
	allocator_reallocate_f reallocate;
	allocator_free_f free;
	uint32_t block_size;
} allocator_t;

#endif /*_UTIL_ALLOCATOR_H_*/
