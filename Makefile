# Copyright 2010 Scott Mohekey
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

CFLAGS += -g -std=c99 -D_GNU_SOURCE -DENABLE_DEBUG -DENABLE_TESTS -Iinc 
LDFLAGS += -L.

OBJS += src/exception.o
OBJS += src/buffer.o
OBJS += src/string.o
OBJS += src/pool.o
OBJS += src/list.o
OBJS += src/hashtable.o
OBJS += src/allocator.o
OBJS += src/allocator-stdlib.o
OBJS += src/tokeniser.o
OBJS += src/test-suite.o
OBJS += src/test-case.o
OBJS += src/test-main.o

SRCS += $(OBJS:.o=.c)
DEPS += $(OBJS:.o=.d)

ifeq ($(shell getconf LONG_BIT),64)
	LIB_DEST = /usr/lib64
else
	LIB_DEST = /usr/lib
endif

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
%.o: %.d %.c

LIBS := libutil.so
EXES := test

libutil_so_OBJS := $(OBJS)

test_OBJS := $(OBJS) src/test-main.o

all: $(SRCS) $(DEPS) $(OBJS) $(ARCS) $(LIBS) $(EXES)

clean: $(foreach lib,$(LIBS),clean_$(subst .,_,$(lib)))
	rm -f $(DEPS)
	rm -f $(OBJS)
	rm -f $(EXES)
	rm -f $(LIBS)
	rm -f $(ARCS)

install: all
	cp libutil.so $(LIB_DEST)
	cp -fr inc/* /usr/include

define LIB_target
$(1): $$($(2)_OBJS)
	gcc $(LDFLAGS) -shared $$($(2)_OBJS) $$($(2)_LIBS:%=-l%) -o $(1)

clean_$(2):
	rm -f $$($(2)_OBJS)
endef 

$(foreach lib,$(LIBS),$(eval $(call LIB_target,$(lib),$(subst .,_,$(lib)))))

define ARC_template
$(1): $$($(2)_OBJS)
	ar sr $(1) $$($(2)_OBJS)

clean_$(2):
	rm -f $$($(2)_OBJS)
endef 

$(foreach arc,$(ARCS),$(eval $(call ARC_template,$(arc),$(subst .,_,$(arc)))))

define EXE_template
$(1): $$($(2)_OBJS)
	gcc $(LDFLAGS) -o $(1) $$($(2)_OBJS) $$($(2)_LIBS:%=-l%)

clean_$(2):
	rm -f $$($(2)_OBJS)
endef
     
$(foreach exe,$(EXES),$(eval $(call EXE_template,$(exe),$(subst .,_,$(exe)))))

indent:
	indent -br -brs -cdw -ce -di0 -i2 -ts2 -ut -npsl -cli2 -cbi0


