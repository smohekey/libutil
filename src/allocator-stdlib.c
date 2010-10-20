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

#include <stdlib.h>
#include <string.h>

#include <util/allocator.h>
#include <util/test-suite.h>

static void* _allocator_stdlib_allocate( uint64_t size ) {
	void* allocation = malloc( size );
	
	if( allocation ) {
		memset( allocation, 0, size );
		
		return allocation;
	}
	
	return NULL;
}

static void* _allocator_stdlib_reallocate( void* allocation, uint64_t size ) {
	return realloc( allocation, size );
}

static void _allocator_stdlib_free( void* allocation ) {
	free( allocation );
}

allocator_t allocator_stdlib = {
	&_allocator_stdlib_allocate,
	&_allocator_stdlib_reallocate,
	&_allocator_stdlib_free
};

TEST_SUITE(allocator_stdlib,{},{})

TEST_CASE(allocator_stdlib, allocate, {
	FAIL_IF(NULL == allocator_stdlib.allocate( 100 ));
})

TEST_CASE(allocator_stdlib, reallocate, {
        void* m = allocator_stdlib.allocate( 100 );

        FAIL_IF(NULL == allocator_stdlib.reallocate( m, 200 ));
})

