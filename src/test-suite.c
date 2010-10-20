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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <util/test-suite.h>
#include <util/exception.h>

#ifdef ENABLE_TESTS

static test_suite_t* _suites = NULL;

void test_suite_register( test_suite_t* suite ) {
	suite->next = _suites;
	_suites = suite;
}

test_suite_t* test_suite_find( const char* name ) {
	test_suite_t* suite = _suites;

	while( suite ) {
		if( 0 == strcmp( name, suite->name ) ) {
			return suite;
		}

		suite = suite->next;
	}

	return NULL;
}

static int _test_case_run(test_suite_t* suite, test_case_t* tcase) {
	int result = TEST_PASSED;

	EXCEPTION_TRY
		case EXCEPTION_CODE: {
			suite->setup(suite);

			result = tcase->impl(tcase);

			suite->cleanup(suite);

			break;
		}

		default: {
			printf("Exception occurred while running test case: %s\n", exception.message);

			exception_handled();

			result = TEST_FAILED;

			break;
		}

		case EXCEPTION_FINALLY: {
			break;
		}
	EXCEPTION_ENDX

	return result;
}

int test_suite_run( test_suite_t* suite ) {
	test_case_t* tcase = suite->cases;
	int run = 0;
	int failed = 0;

	printf( "Running suite: %s\n", suite->name );
	
	while( tcase ) {
		int result = 0;

		run ++;
		
		printf(" Running case: %s (%s:%d)\n", tcase->name, tcase->file, tcase->line);

		if(0) {
			pid_t child = fork();

			if(child < 0) {
				printf("Couldn't fork child process.\n");
				return 1;
			}

			if(child) {
				int status;

				signal(SIGINT, SIG_IGN);
				wait(&status);
				signal(SIGINT, SIG_DFL);

				if(WIFSIGNALED(status)) {
					result = 1;

					switch(WTERMSIG(status)) {
						case SIGSEGV: {
							printf("Segmentation fault in test case.\n");
						} break;
					}

				} else if(WIFEXITED(status)) {
					result = WEXITSTATUS(status);
				}
			} else {
				exit(_test_case_run(suite, tcase));
			}
		} else {
			result = _test_case_run(suite, tcase);
		}

		printf( " %s\n", result == TEST_PASSED ? "PASSED" : "FAILED" );

		failed += result ? 1 : 0;

		tcase = tcase->next;
	}
	
	printf( "Results for suite %s: run %d, passed %d, failed %d\n", suite->name, run, run - failed, failed );

	return failed;
}

void test_suite_list() {
	test_suite_t* suite = _suites;

	while(suite) {
		printf("suite: %s\n", suite->name);

		test_case_t* tcase = suite->cases;

		while(tcase) {
			printf("\tcase: %s (%s:%d)\n", tcase->name, tcase->file, tcase->line);

			tcase = tcase->next;
		}

		suite = suite->next;
	}
}

void test_suite_run_all( ) {
	int suitesFailed = 0;
	int casesFailed = 0;
	int suitesRun = 0;

	test_suite_t* suite = _suites;

	while( suite ) {
		suitesRun ++;

		int result = test_suite_run( suite );

		suitesFailed += result ? 1 : 0;
		casesFailed += result;

		suite = suite->next;
	}

	if(suitesRun > 1) {
		printf("Results: suites run %d, suites passed %d, suites failed %d, total cases failed %d\n", suitesRun, suitesRun - suitesFailed, suitesFailed, casesFailed);
	}	
}

void test_suite_add_case( test_suite_t* suite, test_case_t* tcase ) {
	tcase->next = suite->cases;

	suite->cases = tcase;
}

#endif /*ENABLE_TESTS*/
