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

#ifndef _TEST_CASE_H_
#define _TEST_CASE_H_

#ifdef ENABLE_TESTS

#define __test __attribute__ ((__section__("tests")))

#include <stdio.h>

typedef struct _test_case test_case_t;

typedef int (*test_case_impl_f)( test_case_t* );

struct _test_case {
	test_case_t* next;

	char* name;
	char* file;
	int line;
	test_case_impl_f impl;
};

#define TEST_CASE(_suite,_name,_impl) \
static int __test __test_case_impl_##_name( test_case_t* tcase ) { \
_impl; \
return TEST_PASSED; \
} \
\
static test_case_t __test_case_##_name = { .name = #_name, .impl = __test_case_impl_##_name, .file = __FILE__, .line = __LINE__ }; \
\
static void __test __attribute__((constructor)) __util_test_case_init_##_name() { \
	test_suite_add_case( &__test_suite_##_suite, &__test_case_##_name ); \
} 

#define TEST_PASSED 0
#define TEST_FAILED 1

#define FAIL_IF(x) \
do { printf("  FAIL_IF(%s)" , #x); if(x) { printf(" failed!\n"); return TEST_FAILED; } else { printf(" passed!\n"); } } while(0)

#define FAIL_UNLESS(x) \
do { printf("  FAIL_UNLESS(%s)", #x); if(!(x)) { printf(" failed!\n"); return TEST_FAILED; } else { printf(" passed!\n"); } } while(0)

#else /*ENABLE_TESTS*/
#define TEST_CASE(_suite,_name,_impl)
#endif /*ENABLE_TESTS*/

#endif /*_TEST_CASE_H_*/
