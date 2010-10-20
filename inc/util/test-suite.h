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

#ifndef _TEST_SUITE_H_
#define _TEST_SUITE_H_

#include <util/test-case.h>

#ifdef ENABLE_TESTS

typedef struct _test_suite test_suite_t;

typedef void (*test_suite_setup_f)( test_suite_t* );
typedef void (*test_suite_cleanup_f)( test_suite_t* );

struct _test_suite {
	test_suite_t* next;

	char* name;
	test_suite_setup_f setup;
	test_suite_cleanup_f cleanup;
	test_case_t* cases;
};

void __test test_suite_register( test_suite_t* suite );
test_suite_t* __test test_suite_find( const char* name );
int __test test_suite_run( test_suite_t* suite);
void __test test_suite_run_all();
void __test test_suite_list();
void __test test_suite_add_case( test_suite_t* suite, test_case_t* tcase );

#define TEST_SUITE(_name,_setup,_cleanup) \
static void __test __test_suite_setup_##_name( test_suite_t* suite ) { \
_setup; \
} \
\
static void __test __test_suite_cleanup_##_name( test_suite_t* suite ) { \
_cleanup; \
} \
\
test_suite_t __test_suite_##_name = { .name = #_name, .setup = __test_suite_setup_##_name, .cleanup = __test_suite_cleanup_##_name }; \
\
static void __test __attribute__((constructor)) __test_suite_init_##_name() { \
	test_suite_register( &__test_suite_##_name ); \
}

#define DECLARE_TEST_SUITE(_name) \
extern test_suite_t __test_suite_##_name;

#else /*ENABLE_TESTS*/
#define TEST_SUITE(_name,_setup,_cleanup)
#define IMPORT_TEST_SUITE(_name)
#endif /*ENABLE_TESTS*/

#endif /*_TEST_SUITE_H_*/
