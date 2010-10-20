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

#include <util/debug.h>
#include <util/pool.h>
#include <util/test-suite.h>
#include <util/allocator-stdlib.h>

typedef struct _pool_allocation_s {
	uint64_t size;
	uint32_t references;
	
	struct _pool_allocation_s* prev;
	struct _pool_allocation_s* next;
} pool_allocation_t;

struct _pool {
	pool_t* parent;
	pool_t* children;
	
	pool_t* prev;
	pool_t* next;
	
	const allocator_t* allocator;
	pool_allocation_t* allocations;
};

TEST_SUITE(pool,,)

TEST_CASE(pool,create_ex,{ 

        pool_t* pool = pool_create_ex( NULL , &allocator_stdlib );
        FAIL_IF( &pool == NULL );
})

pool_t* pool_create_ex( pool_t* parent, const allocator_t* allocator ) {
	pool_t* pool = allocator->allocate( sizeof( pool_t ) );
	
	if( pool ) {
		pool->parent = parent;
		
		if( parent ) {
			if( parent->children ) {
				parent->children->prev = pool;
			}
			pool->next = parent->children;		
			parent->children = pool;
		}
		
		pool->allocator = allocator;
		
	//	LOG_DEBUG( "Created pool_t (%p) with parent of (%p).", pool_t, parent );
		
		return pool;
	}
	
	//static string message = STRING( "Pool allocator couldn't allocate any memory." );
	//exception_throw( &//E_STACK(pool_t), exception_oom );
	return NULL;
}

TEST_CASE(pool,create,{

	pool_t* parent = pool_create_ex( NULL , &allocator_stdlib );
        pool_t* pool = pool_create( parent );

	FAIL_IF ( pool == NULL );
})

pool_t* pool_create( pool_t* parent ) {
	if(parent) {
		return pool_create_ex( parent, parent->allocator );
	} else {
		return pool_create_ex(NULL, &allocator_stdlib);
	}
}

pool_t* pool_create_root() {
	return pool_create(NULL);
}

pool_t* pool_create_root_ex(const allocator_t* allocator) {
	return pool_create_ex(NULL, allocator);
}

TEST_CASE(pool,allocate,{

 	pool_t* pool = pool_create_ex( NULL , &allocator_stdlib );
	void* mem = pool_allocate ( pool , 128 );

	FAIL_IF (mem == NULL);	
	FAIL_IF ( pool->allocations->size != 128);

})

void* pool_allocate( pool_t* pool, uint64_t size ) {
	pool_allocation_t* allocation = pool->allocator->allocate( size + sizeof( pool_allocation_t ) );
	
	//LOG_DEBUG( "Allocated %d bytes from pool_t (%p) as (%p).", size, pool_t, allocation );
	
	allocation->size = size;
	
	if( pool->allocations ) {
		pool->allocations->prev = allocation;
	}
	
	allocation->next = pool->allocations;
	pool->allocations = allocation;
	
	void* memory = allocation;

	return memory + sizeof( pool_allocation_t );
}

void* pool_reallocate( pool_t* pool, void* memory, uint64_t size ) {
	pool_allocation_t* allocation = (pool_allocation_t*)(memory - sizeof(pool_allocation_t));

	allocation = pool->allocator->reallocate(allocation, size + sizeof(pool_allocation_t));
	allocation->size = size;

	if(allocation->prev) {
		allocation->prev->next = allocation;
	} else {
		pool->allocations = allocation;
	}

	if(allocation->next) {
		allocation->next->prev = allocation;
	}

	memory = allocation;

	return memory + sizeof(pool_allocation_t);
}

void* pool_reference(void* memory) {
	pool_allocation_t* allocation = (pool_allocation_t*)(memory - sizeof(pool_allocation_t));

	allocation->references ++;

	return memory;
}

void pool_release(void* memory) {
	pool_allocation_t* allocation = (pool_allocation_t*)(memory - sizeof(pool_allocation_t));

	allocation->references --;
}

TEST_CASE(pool,free,{

        pool_t* pool = pool_create_ex( NULL , &allocator_stdlib );
        void* mem = pool_allocate ( pool , 128 );
	FAIL_IF ( pool->allocations == NULL);

	pool_free( pool, mem );
        FAIL_IF ( pool->allocations != NULL);

})

void pool_free( pool_t* pool, void* memory ) {
	pool_allocation_t* allocation = (pool_allocation_t*)( memory - sizeof( pool_allocation_t ) );
	pool_allocation_t* first;
	
	//LOG_DEBUG( "Freeing allocation (%p) from pool_t (%p).", allocation, pool_t );
	
	for( first = pool->allocations ; first ; first = first->next ) {
		if( first == allocation ) {
			if( allocation->prev ) {
				allocation->prev->next = allocation->next;
			} else {
				pool->allocations = allocation->next;
			}
			
			if( allocation->next ) {
				allocation->next->prev = allocation->prev;
			}
			
			pool->allocator->free( allocation );
			
			return;
		}
	}
	
	//string message = STRING( "Attempted to free invalid pointer." );
	////exception_throw( EXCEPTION_INVALID, &message );
	//exception_throw( &//E_STACK(pool_t), exception_create( NULL, "Attempted to free invalid pointer.", 0 ) );
}

TEST_CASE(pool,delete,{

        pool_t* parent = pool_create_ex( NULL , &allocator_stdlib );
	pool_t* pool = pool_create( parent );

	pool_delete( parent );

        FAIL_IF ( parent->children != NULL);
})


void pool_delete( pool_t* pool ) {
	//LOG_DEBUG( "Deleting pool_t (%p).", pool_t );
	
	// delete all child pools
	pool_t* child = pool->children;
	
	while( child ) {
		pool_t* next = child->next;
		pool_delete( child );
		child = next;
	}
	
	// free all allocations
	pool_allocation_t* allocation = pool->allocations;
	
	while( allocation ) {
		//LOG_DEBUG( "Freeing allocation (%p) from pool_t (%p).", allocation, pool_t );
		
		pool_allocation_t* next = allocation->next;
		
		pool->allocator->free( allocation );
		
		allocation = next;
	}
	
	// remove this pool_t from it's parent
	
	if( pool->next ) {
		pool->next->prev = pool->prev;
	}
	
	if( pool->prev ) {
		pool->prev->next = pool->next;
	} else if( pool->parent ) {
		pool->parent->children = pool->next;
	}
	
	pool->allocator->free( pool );
}

