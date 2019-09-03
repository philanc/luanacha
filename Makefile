
# ----------------------------------------------------------------------
# Use environment variables to override the default options:
#      CC, LUA, CFLAGS, FPIC, LUA_CFLAGS, LDFLAGS
#  for example: CC=mips-linux-gcc LUA_LIB=lua5.2 make
# ----------------------------------------------------------------------

LUA ?= lua
LUA_LIB ?= lua
CC ?= gcc

FPIC ?= -fPIC
CFLAGS ?= -Os

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -bundle -undefined dynamic_lookup
endif

OBJS= luanacha.o monocypher.o randombytes.o

%.o: src/%.c
	$(CC) $(LUA_CFLAGS) $(CFLAGS) $(LDFLAGS) $(FPIC) -c -o $@ $<

compile: $(OBJS)
	$(CC) $(FPIC) -shared -o luanacha.so -l$(LUA_LIB) $(OBJS)

test: compile
	$(LUA) test_luanacha.lua

clean:
	rm -f *.o *.a *.so

install: compile
	mkdir -p $(DESTDIR)
	cp -pR luanacha.so $(DESTDIR)/

.PHONY: clean test
