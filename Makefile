
# ----------------------------------------------------------------------
# adjust the following according to your Lua install
#
#	LUADIR can be defined. In that case, 
#   Lua binary and include files are to be found repectively in 
#   $(LUADIR)/bin and $(LUADIR)/include
#
#	Or LUA and LUAINC can be directly defined as the path of the 
#	Lua executable and the path of the Lua include directory,
#	respectively.

LUADIR ?= ../lua
LUA ?= $(LUADIR)/bin/lua
LUAINC ?= $(LUADIR)/include
LUALIBDIR ?= $(LUADIR)/lib
LUALIB ?= lua

# ----------------------------------------------------------------------

CC ?= gcc
AR ?= ar

INCFLAGS= -I$(LUAINC)
CFLAGS= -Os -fPIC $(INCFLAGS) 
LDFLAGS= -fPIC -L$(LUALIBDIR) -l$(LUALIB)

OBJS= luanacha.o monocypher.o randombytes.o

luanacha.so:  src/*.c src/*.h
	$(CC) -c $(CFLAGS) src/*.c
	$(CC) -shared $(LDFLAGS) -o luanacha.so $(OBJS)

test:  luanacha.so
	$(LUA) test_luanacha.lua
	
clean:
	rm -f *.o *.a *.so

.PHONY: clean test


