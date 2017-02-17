
# ----------------------------------------------------------------------
# adjust the following to the location of your Lua install
#
#   Lua binary and include files are to be found repectively in 
#   $(LUADIR)/bin and $(LUADIR)/include

LUADIR ?= ../lua

# ----------------------------------------------------------------------

CC= gcc
AR= ar
LUA ?= $(LUADIR)/bin/lua

INCFLAGS= -I$(LUADIR)/include
CFLAGS= -Os -fPIC $(INCFLAGS) 
LDFLAGS= -fPIC

OBJS= luanacha.o monocypher.o randombytes.o

luanacha.so:  src/*.c src/*.h
	$(CC) -c $(CFLAGS) src/*.c
	$(CC) -shared $(LDFLAGS) -o luanacha.so $(OBJS)

test:  luanacha.so
	$(LUA) test/test_luanacha.lua
	
clean:
	rm -f *.o *.a *.so

.PHONY: clean test

