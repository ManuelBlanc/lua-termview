
/* usr */
#include "lua_termview.h"
/* lua */
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
/* libc */
#include <stdio.h>
#include <string.h>

/* Lua 5.2 */
#ifndef LUA_OK
#define LUA_OK	(0)
#endif

#if 0
/* Lua traceback (from the Lua 5.1 executable) */
static int traceback (lua_State *L) {
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}
#endif

static int traceback (lua_State *L) {
	const char *msg = lua_tostring(L, 1);
	if (msg)
		luaL_traceback(L, L, msg, 1);
	else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
		if (!luaL_callmeta(L, 1, "__tostring"))  /* try its 'tostring' metamethod */
			lua_pushliteral(L, "(no error message)");
	}
	return 1;
}

int main(int argc, char const *argv[])
{
	lua_State* L;
	int ret;

	if (argc < 2) {
		fprintf(stderr, "No Lua file specified.\n");
		return 1;
	}

	L = luaL_newstate();
	if (!L) {
		fprintf(stderr, "Couldn't create Lua state.\n");
		return 1;
	}

	luaL_openlibs(L);
	lua_pushcfunction(L, luaopen_termview);
	if (LUA_OK != lua_pcall(L, 0, 1, 0)) {
		fprintf(stderr, "error in luaopen_termview: %s\n", lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}
	lua_setglobal(L, "view");

	/* Error traceback generator */
	lua_pushcfunction(L, traceback);

	ret = luaL_loadfile(L, argv[1]);
	if (!ret) ret = lua_pcall(L, 0, LUA_MULTRET, -2);

	switch (ret) {
		case LUA_ERRFILE:   /* cannot open/read the file. */
		case LUA_ERRSYNTAX: /* syntax error during pre-compilation. */
		case LUA_ERRMEM:    /* memory allocation error. */
		case LUA_ERRRUN:    /* a runtime error. */
		case LUA_ERRERR:    /* error while running the error handler function. */
			fprintf(stderr, "%s\n", lua_tostring(L, -1));
			lua_close(L);
			return 1;
	}
	lua_close(L);
	fprintf(stderr, "OK.\n");
	return 0;
}
