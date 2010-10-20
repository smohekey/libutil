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

#include <util/pool.h>
#include <util/hashtable.h>
#include <util/exception.h>

#include <util/test-suite.h>

uint32_t hashtable_hash_mod32(uint32_t tableSize, void* key) {
	return ((uint32_t)key) % tableSize;
}

#ifdef ENABLE_TESTS
pool_t* _test_pool;
#endif

TEST_SUITE(hashtable,{
	_test_pool = pool_create_root();
},{
	pool_delete(_test_pool);
})

/** \brief Creates a hashtable of the given size,
 * using the given hash function to generate hashes from keys.
 * 
 * \param pool The pool used for memory allocation.
 * \param tableSize The size of the table.
 * \param hash The hashing function to use.
 * \returns The newly created hashtable, or NULL if a memory allocation failed.
 */
 
hashtable_t* hashtable_create(pool_t* pool, uint32_t tableSize, hashtable_hash_f hash) {
	pool = pool_create(pool);

	hashtable_t* hashtable = (hashtable_t*)pool_allocate(pool, sizeof(hashtable_t) + (sizeof(hashtable_element_t*) * tableSize));

	hashtable->pool = pool;
	hashtable->tableSize = tableSize;
	hashtable->elements = (void*)(hashtable + 1);
	hashtable->hash = hash;
	
	return hashtable;
}

TEST_CASE(hashtable,create,{
	hashtable_t* hashtable = hashtable_create(_test_pool, 1024, hashtable_hash_mod32);

	FAIL_IF(hashtable == NULL);
	FAIL_IF(hashtable->elements == NULL);
})

/** \brief Checks to see whether an element with the given key exists in the hashtable.
 * 
 * \param hashtable The hashtable to check in.
 * \param key The key to check for.
 * \returns TRUE if an element with the given key exists, otherwise FALSE.
 */

bool hashtable_contains(hashtable_t* hashtable, void* key) {
	return NULL != hashtable_get(hashtable, key);
}

TEST_CASE(hashtable,contains,{
	hashtable_t* hashtable = hashtable_create(_test_pool, 1024, hashtable_hash_mod32);

	uint32_t key1 = 1;
	uint32_t key2 = 2;
	uint32_t value = 1234;

	hashtable_put(hashtable, (void*)key1, (void*)value);

	FAIL_UNLESS((uint32_t)hashtable_get(hashtable, (void*)key1) == value);
	FAIL_UNLESS(NULL == hashtable_get(hashtable, (void*)key2));
})

/** \brief Returns the value associated with the given key from the hashtable.
 * 
 * If no value exists associated with that key, hashtable_get returns NULL.
 * 
 * \param hashtable The hashtable to get the value from.
 * \param key The key to get the value for.
 * \returns The value of the given key if it exists, otherwise NULL.
 */

void* hashtable_get(hashtable_t* hashtable, void* key) {
	uint32_t hash = hashtable->hash(hashtable->tableSize, key);
	
	hashtable_element_t* element = hashtable->elements[hash];
	
	while(element) {
		if(element->key == key) {
			return element->value;
		}
		
		element = element->next;	
	}
	
	return NULL;
}

/** \brief Adds the given value to the hashtable with the given key.
 * 
 * \param hashtable The hashtable to add the value to.
 * \param key The key to associate with the value.
 * \param value The value to add to the hashtable.
 * 
 * \todo Check for previously existing values with the given key, and fail if any
 * are present.
 */

void hashtable_put(hashtable_t* hashtable, void* key, void* value) {
	hashtable_element_t* newElement = pool_allocate(hashtable->pool, sizeof(hashtable_element_t));
	
	newElement->key = key;
	newElement->value = value;
	
	uint32_t hash = hashtable->hash(hashtable->tableSize, key);
	
	hashtable_element_t* element = hashtable->elements[hash];
	
	if(element) {
		newElement->next = element;
		element->prev = newElement;
	}
	
	hashtable->elements[hash] = newElement;
}

/** \brief Removes the value associated with the given key from the hashtable.
 * 
 * \param hashtable The hashtable to remove the value from.
 * \param key The key associated with the value you want to remove.
 */

void hashtable_remove(hashtable_t* hashtable, void* key) {
	uint32_t hash = hashtable->hash(hashtable->tableSize, key);
	
	hashtable_element_t* element = hashtable->elements[hash];
	
	while(element) {
		if(element->key == key) {
			if(element->next)
				element->next->prev = element->prev;
				
			if(element->prev)
				element->prev->next = element->next;
				
			pool_free(hashtable->pool, element);
			
			return;
		}
		
		element = element->next;	
	}
	
	return;
}

/** \brief Frees all memory allocated for use by the hashtable.
 * 
 * \param hashtable The hashtable to delete.
 */
 
void hashtable_delete(hashtable_t* hashtable) {
	pool_delete(hashtable->pool);	
}

