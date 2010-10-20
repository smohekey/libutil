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

#ifndef _UTIL_HASHTABLE_H_
#define _UTIL_HASHTABLE_H_

#include <stdbool.h>

#include <util/pool.h>

typedef struct _hashtable_element_s {
	void* key;
	void* value;
	
	struct _hashtable_element_s* next;
	struct _hashtable_element_s* prev;
} hashtable_element_t;

typedef uint32_t (*hashtable_hash_f)(uint32_t tableSize, void* key);

typedef struct _hashtable_s {
	pool_t* pool;
	
	hashtable_hash_f hash;
	hashtable_element_t** elements;

	uint32_t tableSize;
} hashtable_t;

hashtable_t* hashtable_create(pool_t* pool, uint32_t tableSize, hashtable_hash_f hash);

bool hashtable_contains(hashtable_t* hashtable, void* key);
void* hashtable_get(hashtable_t* hashtable, void* key);
void hashtable_put(hashtable_t* hashtable, void* key, void* value);
void hashtable_remove(hashtable_t* hashtable, void* key);

void hashtable_delete(hashtable_t* hashtable);

#endif /*_UTIL_HASHTABLE_H_*/

