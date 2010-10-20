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

#ifndef UTIL_LIST_H_
#define UTIL_LIST_H_

#include <util/pool.h>
#include <util/exception.h>

typedef struct _list_s list_t;

struct _list_s {
	list_t* prev;
	list_t* next;
};

extern list_t* list_create(pool_t* pool);

static inline void list_init(list_t* list) {
	list->prev = list->next = list;
}

static inline int8_t list_is_empty( list_t* list ) {
	return ( list->prev == list->next ) && ( list->prev == (list_t*)list );
}

static inline list_t* list_remove( list_t* node ) {
	node->prev->next = node->next;
	node->next->prev = node->prev;
	return node;
} 

static inline void list_insert_before( list_t* before, list_t* node ) {
	node->next = before;
	node->prev = before->prev;
	before->prev->next = node;
	before->prev = node;	
}

static inline void list_insert_after( list_t* after, list_t* node ) {
	node->next = after->next;
	node->prev = after;
	after->next->prev = node;
	after->next = node;
}

static inline void list_append( list_t* list, list_t* node ) {
	list_insert_before( list, node );
}

static inline void list_prepend( list_t* list, list_t* node ) {
	list_insert_after( list, node );
}

static inline list_t* list_remove_head( list_t* list ) {
	return list_remove( list->next );
}

static inline list_t* list_remove_tail( list_t* list ) {
	return list_remove( list->prev );
}

static inline list_t* list_element_at(list_t* list, uint32_t index) {
	int orig_index = index;
	list_t* element = list->next;

	while(element && index > 0) {
		element = element->next;
		index --;
		if (element == list)
			exception_throw(EXCEPTION_INDEX_OUT_OF_RANGE, "Requested index is out of range: %d.", orig_index);
	}

	if(!element) {
		exception_throw(EXCEPTION_INDEX_OUT_OF_RANGE, "List node does not exist: %d.", index);
	}

	return element;
}

#define list_foreach(list, item_type, item) for(item_type item = (item_type)(list)->next; (list_t*)(list) != (list_t*)item; item = (item_type)((list_t*)item)->next) 

//extern void list_print( list_t* list );

#endif /*UTIL_LIST_H_*/

