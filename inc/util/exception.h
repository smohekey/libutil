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

#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include <setjmp.h>
#define EXCEPTION_CONTEXT				jmp_buf
#define EXCEPTION_SAVE_CONTEXT(context)		setjmp(*(context))
#define EXCEPTION_RESTORE_CONTEXT(context, value)	longjmp(*(context), value)

typedef struct _exception_record_s {
	struct _exception_record_s*	next;
	EXCEPTION_CONTEXT		context;
	char*				message;
	int				value;
	int				is_handled;
	enum {
		ExceptionState_Code, 
		ExceptionState_Handling, 
		ExceptionState_Finally
	} state;
} exception_record_t;

#define EXCEPTION_NOEX			0 // no exception
#define EXCEPTION_CODE			EXCEPTION_NOEX // value for the code block
#define EXCEPTION_FINALLY		-1 // value for the finally block

#define EXCEPTION_NOT_IMPLEMENTED	-10
#define EXCEPTION_NOT_SUPPORTED		-11
#define EXCEPTION_INVALID_STATE		-12
#define EXCEPTION_INVALID_OPERATION	-13
#define EXCEPTION_EOF			-14
#define EXCEPTION_PARSE_ERROR		-15
#define EXCEPTION_KEY_NOT_FOUND		-16
#define EXCEPTION_INDEX_OUT_OF_RANGE	-17

#define EXCEPTION_USER_BASE		-1000

void exception_link_record(exception_record_t* record);
int  exception_unlink_record_with_return(exception_record_t* record);
int  exception_unlink_record(exception_record_t* record);

#define EXCEPTION_TRY    while (1) { exception_record_t exception; exception_link_record(&exception); switch (EXCEPTION_SAVE_CONTEXT(&exception.context)) {
#define EXCEPTION_END    } { int x = exception.value; if (exception_unlink_record_with_return(&exception)) return exception.value; else if (x <= 0) break; } }
#define EXCEPTION_ENDX   } if (exception_unlink_record(&exception) == 0) break; }
#define EXCEPTION_VALUE  exception.value

void exception_throw(int value, char* message, ...);
void exception_reexecute(void);
void exception_handled(void);

#endif

