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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <util/string.h>

#define BUFFER_SIZE 128

string_t* string_create(pool_t* pool) {
	string_t* string = (string_t*)pool_allocate(pool, sizeof(string_t));
	
	string->pool = pool;
	string->buffer = (char*)pool_allocate(string->pool, BUFFER_SIZE);
	memset(string->buffer, 0, BUFFER_SIZE);
	string->length = BUFFER_SIZE;
	string->index = 0;

	return string;
}

void string_delete(string_t* string) {
	pool_free(string->pool, string->buffer);
	pool_free(string->pool, string);
}

void string_clear( string_t* string ) {
	string->index = 0;
	memset(string->buffer, 0, string->length);
}

int string_grow( string_t* string ) {
	string->buffer = (char*)pool_reallocate(string->pool, string->buffer, string->length + BUFFER_SIZE);
	
	string->length += BUFFER_SIZE;

	memset( string->buffer + string->index, 0, string->length - string->index );

	return string->length - string->index;
}

void string_append( string_t* string, const char* value ) {
	int length = strlen( value );

	while( ( string->length - string->index ) < length + 1 )
	{
		string_grow( string );
	}

	strcpy( string->buffer + string->index, value );

	string->index += length;
}

void string_appendn(string_t* string, const char* value, size_t length) {
	while(( string->length - string->index ) < length + 1) {
		string_grow(string);
	}

	strncpy(string->buffer + string->index, value, length);
	string->index += length;
}

void string_append_char( string_t* string, char character ) {
	if( ( string->length - string->index ) < 2 )
	{
		string_grow( string );
	}

	string->buffer[ string->index ++ ] = character;
}

void string_appendf( string_t* string, char* format, ... ) {
	int available = 0;
	int needed = 0;

	while( 1 )
	{
		va_list ap;
		va_start( ap, format );

		available = string->length - string->index;

		needed = vsnprintf( string->buffer + string->index, available, format, ap );

		va_end( ap );

		if( needed > -1 && needed < available )
		{
			string->index += needed;
			return;
		}

		while( available <= needed )
		{
			available = string_grow( string );
		}
	}
}

const char* strnchr(const char* s, size_t length, char c) {
	size_t i;

	for(i = 0; i < length; i++) {
		if(s[i] == c) {
			return s + i;
		}
	}

	return 0;
}

