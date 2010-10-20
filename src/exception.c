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
#include <stdarg.h>
#include <string.h>

#include <util/exception.h>

void _exception_error_exit(char* message, int code) {
   printf("%s\n", message);
   exit(code);
}

static void *_exception_single_tls_value = NULL;

#define _exception_alloc_tls()           0
#define _exception_get_tls(index)        _exception_single_tls_value
#define _exception_set_tls(index, value) _exception_single_tls_value = value

// End of OS dependent functions


static int _exception_tls_index = -1;  // assume -1 is an illegal value

///////////////////////////////////////
static void _exception_init(void)
{
   _exception_tls_index = _exception_alloc_tls();

   if (_exception_tls_index == -1)
      _exception_error_exit("Unable to allocate exception TLS", 1);
}

///////////////////////////////////////
void exception_throw(int value, char* format, ...) {
	exception_record_t* exception;

	if(_exception_tls_index == -1)
		_exception_init();

#ifdef ENABLE_DEBUG
	if (value >= EXCEPTION_FINALLY)
		_exception_error_exit("Cannot raise positive or reserved error code", value);
#endif

	int available = 0;
	int offset = 0;
	int length = strlen(format);
	int needed = 0;

	char* message = malloc(length);

	while(1) {
		va_list ap;
		va_start(ap, format);

		available = length - offset;

		needed = vsnprintf(message + offset, available, format, ap);

		va_end(ap);

		if(needed > -1 && needed < available) {
			break;
		}

		length += needed;

		message = realloc(message, length);
	}
	
	exception = _exception_get_tls(_exception_tls_index);

	if(exception) {
		exception->message = message;

		while(exception != NULL) {
			exception->is_handled = 0;

			switch(exception->state) {
				case ExceptionState_Code:               // execute the handler
					exception->value = value;
					exception->state  = ExceptionState_Handling;
					EXCEPTION_RESTORE_CONTEXT(&exception->context, value);

				case ExceptionState_Handling:           // reraise, do finally first
					exception->value = value;
					exception->state  = ExceptionState_Finally;
					EXCEPTION_RESTORE_CONTEXT(&exception->context, EXCEPTION_FINALLY);

				case ExceptionState_Finally:            // throw within finally; propagate
#ifdef ENABLE_DEBUG
					_exception_error_exit("Throw within finally", 1);
#endif
					_exception_set_tls(_exception_tls_index, exception = exception->next);
					break;

				default:
					_exception_error_exit("Corrupted exception handler chain", 1);
			}
		}
	}
   
	char error[1024];

	strcpy(error, "Unhandled exception: ");
	strncat(error, message, 1024 - strlen(error));

	_exception_error_exit(error, value);
}

///////////////////////////////////////
void exception_reexecute(void) {
	exception_record_t * exception = _exception_get_tls(_exception_tls_index);

#ifdef ENABLE_DEBUG
	if (exception == NULL)
		_exception_error_exit("no exception handler frame in XReExecute", 1);
#endif

	exception->value = EXCEPTION_NOEX;
	exception->state = ExceptionState_Code;
	exception->is_handled = 1;

	EXCEPTION_RESTORE_CONTEXT(&exception->context, EXCEPTION_CODE);
}

///////////////////////////////////////
void exception_link_record(exception_record_t * exception) {
	if(_exception_tls_index == -1)
		_exception_init();

	exception->next = _exception_get_tls(_exception_tls_index);
	_exception_set_tls(_exception_tls_index, exception);
	exception->value = EXCEPTION_NOEX;
	exception->state = ExceptionState_Code;
	exception->is_handled = 1;
}

///////////////////////////////////////
void exception_handled(void) {
	exception_record_t * exception = _exception_get_tls(_exception_tls_index);

#ifdef ENABLE_DEBUG
	if(exception == NULL)
		_exception_error_exit("no exception handler frame in XReExecute", 1);

		switch(exception->state) {
			case ExceptionState_Handling:
				break;

			case ExceptionState_Code:
			case ExceptionState_Finally:
				_exception_error_exit("Internal: exception handled outside handler", 1);

			default:
				_exception_error_exit("Corrupted exception handler chain", 1);
		}
#endif

	exception->value    = EXCEPTION_NOEX;
	exception->state     = ExceptionState_Code;
	exception->is_handled = 1;
}

///////////////////////////////////////
int exception_unlink_record(exception_record_t * exception) {
#ifdef ENABLE_DEBUG
	if(_exception_get_tls(_exception_tls_index) != exception)
		_exception_error_exit("Corrupted exception handler chain", 1);
#endif

	switch(exception->state) {
		case ExceptionState_Handling:              // exception is now handled
		case ExceptionState_Code:                  // no exception
			exception->state = ExceptionState_Finally;
			EXCEPTION_RESTORE_CONTEXT(&exception->context, EXCEPTION_FINALLY);

		case ExceptionState_Finally:               // we are done
			_exception_set_tls(_exception_tls_index, exception->next);

			if(exception->is_handled)
				return 0;              // continue after XTRY block
			else
				exception_throw(exception->value, exception->message);    // propagate exception 
			
			break;

		default:
			_exception_error_exit("Corrupted exception handler chain", 1);
	}

	return 0;                       // never gets here
}

///////////////////////////////////////
int exception_unlink_record_with_return(exception_record_t * exception) {
#ifdef ENABLE_DEBUG
	if(_exception_get_tls(_exception_tls_index) != exception)
		_exception_error_exit("Corrupted exception handler chain", 1);
#endif

	switch(exception->state) {
		case ExceptionState_Handling:              // exception is now handled
		case ExceptionState_Code:                  // no exception
			exception->state = ExceptionState_Finally;
			EXCEPTION_RESTORE_CONTEXT(&exception->context, EXCEPTION_FINALLY);

		case ExceptionState_Finally:               // we are done
			_exception_set_tls(_exception_tls_index, exception->next);

			if(exception->is_handled)
				return 0;              // continue after XTRY block
			else if (exception->next != NULL)
				exception_throw(exception->value, exception->message); // propagate exception 
			else
				return 1;

			break;

		default:
			_exception_error_exit("Corrupted exception handler chain", 1);
	}

	return 0;
}

