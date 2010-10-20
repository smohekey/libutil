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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <util/debug.h>
#include <util/test-suite.h>
#include <util/tokeniser.h>
#include <util/exception.h>

#ifdef ENABLE_TESTS
pool_t* _test_pool;
#endif

TEST_SUITE(tokeniser,{ _test_pool = pool_create_root();}
		, {})

void skip_space(tokeniser_t* tokeniser) {
	if(!(tokeniser->flags & TOKENISER_BINARY) && !(tokeniser->flags & TOKENISER_WHITESPACE)) {
		while(tokeniser->index + 1 < tokeniser->length && tokeniser->buffer[tokeniser->index + 1] && isspace(tokeniser->buffer[tokeniser->index + 1])) {
			tokeniser->index ++;
		}
	}
}

tokeniser_t* tokeniser_create(pool_t* pool, uint8_t flags, const char* buffer) {
	return tokeniser_createn(pool, flags, buffer, strlen(buffer));
}

tokeniser_t* tokeniser_createn(pool_t* pool, uint8_t flags, const char* buffer, size_t length) {
	tokeniser_t* tokeniser = (tokeniser_t*)pool_allocate(pool, sizeof(tokeniser_t));

	tokeniser_initn(tokeniser, flags, buffer, length);

	return tokeniser;
}

void tokeniser_init(tokeniser_t* tokeniser, uint8_t flags, const char* buffer) {
	tokeniser_initn(tokeniser, flags, buffer, strlen(buffer));
}

void tokeniser_initn(tokeniser_t* tokeniser, uint8_t flags, const char* buffer, size_t length) {
	tokeniser->flags = flags;
	tokeniser->buffer = buffer;
	tokeniser->index = -1;
	tokeniser->length = length;
}

void tokeniser_enable_flags(tokeniser_t* tokeniser, uint8_t flags) {
	tokeniser->flags |= flags;
}

void tokeniser_disable_flags(tokeniser_t* tokeniser, uint8_t flags) {
	tokeniser->flags &= ~flags;
}

int8_t tokeniser_next(tokeniser_t* tokeniser) {
	skip_space(tokeniser);

	if(tokeniser->index + 1 >= tokeniser->length) {
		exception_throw(EXCEPTION_EOF, "Unexpected end of file, after %d bytes.", tokeniser->index);
	}
	
	tokeniser->index ++;

	D(
		char c = tokeniser->buffer[tokeniser->index];

		if(isprint(c)) {
			printf("next: 0x%02hhx (%d) (%c)\n", c, c, c);
		} else {
			printf("next: 0x%02hhx (%d)\n", c, c);
		}
	);

	return tokeniser->buffer[tokeniser->index];
}

int8_t tokeniser_current(tokeniser_t* tokeniser) {
	return tokeniser->buffer[tokeniser->index];
}

int8_t tokeniser_peek(tokeniser_t* tokeniser) {
	skip_space(tokeniser);

	if(tokeniser->index + 1 >= tokeniser->length) {
		return -1;
	}

	return tokeniser->buffer[tokeniser->index + 1];
}

void tokeniser_expect(tokeniser_t* tokeniser, char expected) {
	skip_space(tokeniser);

	if(tokeniser->index + 1 >= tokeniser->length) {
		exception_throw(EXCEPTION_EOF, "Expected '%c', found end of file, after %d bytes.", expected, tokeniser->index);
	}

	tokeniser->index ++;

	if(tokeniser->buffer[tokeniser->index] != expected) {
		char near[21];

		memset(near, 0, sizeof(near));

		strncpy(near, tokeniser->buffer + ((tokeniser->index - 10) >= 0 ? (tokeniser->index - 10) : 0), 20);

		exception_throw(EXCEPTION_PARSE_ERROR, "Expected '%c', found '%c' at %d near '%s'.", expected, tokeniser_current(tokeniser), tokeniser->index, near);
	}
}

void tokeniser_copy(tokeniser_t* tokeniser, void* dest, size_t length) {
	while(length-- > 0) {
		*(char*)dest++ = tokeniser_next(tokeniser);
	}
}

TEST_CASE(tokeniser,main,{
	uint8_t* test_data = "asdfasdfasdfaoioqwrweiqrq qwerquiwhyuiasffq!";
	tokeniser_t* tok = tokeniser_create(_test_pool, TOKENISER_BINARY, test_data);
	char lastChar = '\0';
	while (tokeniser_peek(tok) != -1)
	{
		lastChar = tokeniser_next(tok);
	}
	FAIL_IF(lastChar != '!');
})

