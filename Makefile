
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

# ----------------------------------------------------------------------

CC ?= gcc
AR ?= ar

INCFLAGS= -I$(LUAINC)
CFLAGS= -Os -fPIC $(INCFLAGS) 

# link flags for linux
LDFLAGS= -shared -fPIC    

# link flags for OSX
# LDFLAGS=  -bundle -undefined dynamic_lookup -fPIC    

OBJS= luanacha.o monocypher.o randombytes.o

luanacha.so:  src/*.c src/*.h
	$(CC) -c $(CFLAGS) src/*.c
	$(CC)  $(LDFLAGS) -o luanacha.so $(OBJS)

test:  luanacha.so
	$(LUA) test_luanacha.lua
	
clean:
	rm -f *.o *.a *.so

.PHONY: clean test


