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
#include <stdio.h>

#include <util/test-suite.h>

__attribute__((weak)) int main( int argc, char* argv[] ) {
	char doFork = 0;

	if( argc > 1 ) {
		for( int i = 1 ; i < argc ; i ++ ) {
			if(argv[i][0] == '-') {
				switch(argv[i][1]) {
					case 'f': {
						doFork = 1;

						break;
					}

					case 'l': {
						test_suite_list();

						return 0;
					}

					default: {
						printf( "Unrecognised option %s\n", argv[i] );
					}
				}
			} else {
				test_suite_t* suite = test_suite_find( argv[ i ] );
			
				if( suite ) {
					test_suite_run(suite);

					exit(0);
				} else {
					printf( "Couldn't find a test suite called %s.\n", argv[ i ] );	

					exit(1);
				}
			}
		}
	}

	// else {
		test_suite_run_all();
	//}
}
