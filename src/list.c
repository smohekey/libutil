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

#include <util/list.h>

#include <util/test-suite.h>

list_t* list_create(pool_t* pool) {
	list_t* list = (list_t*)pool_allocate(pool, sizeof(list_t));

	list_init(list);

	return list;
}

/* TODO: More testing */
#ifdef ENABLE_TESTS

pool_t* _test_pool;

TEST_SUITE(list,{
		_test_pool = pool_create_root();
		},
		{
		});

TEST_CASE(list,create,{
	list_t* list = list_create(_test_pool);
	FAIL_IF(list == NULL);
	FAIL_IF(list->prev != list->next);
	FAIL_IF(list->prev != list);
	FAIL_IF(!list_is_empty(list));
});

TEST_CASE(list,getset,{
	list_t* list = list_create(_test_pool);
	list_t* node1 = list_create(_test_pool);
	list_t* node2 = list_create(_test_pool);
	list_t* node3 = list_create(_test_pool);
	list_append(list, node1);
	FAIL_IF(list_element_at(list, 0) != node1);
	list_prepend(list, node2);
	FAIL_IF(list_element_at(list, 0) != node2);
	FAIL_IF(list_element_at(list, 1) != node1);
	list_append(list, node3);
	FAIL_IF(list_element_at(list, 0) != node2);
	FAIL_IF(list_element_at(list, 1) != node1);
	FAIL_IF(list_element_at(list, 2) != node3);

});

TEST_CASE(list,remove,{
	int i;
	list_t* list[5];
	for (i = 0; i < 5; i++)
		list[i] = list_create(_test_pool);
	list_append(list[0], list[1]);
	list_append(list[0], list[2]);
	list_append(list[0], list[3]);
	list_append(list[0], list[4]);
	FAIL_IF(list_remove_head(list[0]) != list[1]);
	FAIL_IF(list_element_at(list[0], 0) != list[2]);
	FAIL_IF(list_element_at(list[0], 2) != list[4]);
	FAIL_IF(list_remove_tail(list[0]) != list[4]);
	list_append(list[0], list[3]);
	FAIL_IF(list_element_at(list[0], 0) != list[2]);
	FAIL_IF(list_element_at(list[0], 2) != list[3]);
});

#endif

 
