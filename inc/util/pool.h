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

#ifndef _UTIL_POOL_H_
#define _UTIL_POOL_H_

#include <stdint.h>

#include <util/allocator.h>

/**
 *
 */
typedef struct _pool pool_t;

/**
 *
 */
pool_t* pool_create( pool_t* parent );
pool_t* pool_create_root();

/**
 *
 */
pool_t* pool_create_ex( pool_t* parent, const allocator_t* allocator );
pool_t* pool_create_root_ex(const allocator_t* allocator);

/**
 *
 */
void* pool_allocate( pool_t* pool, uint64_t size );

/**
 *
 */
void* pool_reallocate( pool_t* pool, void* memory, uint64_t size );

void* pool_reference(void* memory);
void pool_release(void* memory);


void pool_free( pool_t* pool, void* memory );

/**
 *
 */
void pool_clear( pool_t* pool );

/**
 *
 */

typedef int (*pool_cleanup_f)( pool_t* pool, void* data );

void pool_add_cleanup( pool_t* pool, pool_cleanup_f* fn, void* data );
void pool_remove_cleanup( pool_t* pool, pool_cleanup_f* fn );

/**
 *
 */
void pool_delete( pool_t* pool );

#endif /*_UTIL_POOL_H_*/
