
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

OBJS= luamonocypher.o monocypher.o 

luamonocypher.so:  src/*.c src/*.h
	$(CC) -c $(CFLAGS) src/*.c
	$(CC) -shared $(LDFLAGS) -o luamonocypher.so $(OBJS)

test:  luamonocypher.so
	$(LUA) test/test_luamonocypher.lua
	
clean:
	rm -f *.o *.a *.so

.PHONY: clean test

