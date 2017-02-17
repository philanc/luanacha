// Copyright (c) 2017  Phil Leblanc  -- see LICENSE file
// ---------------------------------------------------------------------

/*

luanacha

This is a Lua wrapper for the Monocypher library by Loup Vaillant.
http://loup-vaillant.fr/projects/monocypher/

luanachaAPI:

func(...)
	description...

*/

#define LUANACHA_VERSION "luanacha-0.1"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "monocypher.h"

//----------------------------------------------------------------------
// compatibility with Lua 5.2  --and lua 5.3, added 150621
// (from roberto's lpeg 0.10.1 dated 101203)
//
#if (LUA_VERSION_NUM >= 502)

#undef lua_equal
#define lua_equal(L,idx1,idx2)  lua_compare(L,(idx1),(idx2),LUA_OPEQ)

#undef lua_getfenv
#define lua_getfenv	lua_getuservalue
#undef lua_setfenv
#define lua_setfenv	lua_setuservalue

#undef lua_objlen
#define lua_objlen	lua_rawlen

#undef luaL_register
#define luaL_register(L,n,f) \
	{ if ((n) == NULL) luaL_setfuncs(L,f,0); else luaL_newlib(L,f); }

#endif

//----------------------------------------------------------------------
// lua binding   (all LuaNacha exposed functions are prefixed with "ln_")



//----------------------------------------------------------------------
// 


//------------------------------------------------------------
// lua library declaration
//
static const struct luaL_Reg luanachalib[] = {
	{"fname", func},
	
	{NULL, NULL},
};

int luaopen_luanacha(lua_State *L) {
	luaL_register (L, "luanacha", luanachalib);
    // 
    lua_pushliteral (L, "VERSION");
	lua_pushliteral (L, LUANACHA_VERSION); 
	lua_settable (L, -3);
	return 1;
}
