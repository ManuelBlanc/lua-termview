

/* user */
#include "global.h"
#include "view.h"
#include "lua_termview.h"
/* lua */
#include "lua.h"
#include "lauxlib.h"

static View check_view(lua_State *L, int pos) {
	return (View) luaL_checkudata(L, pos, VIEW_MT);
}
#define RET_BOOL(expr) do{ lua_pushboolean((L), OK == (expr)); return 1; }while(0)

static int lua_viewG_start(lua_State *L) {
	UNUSED(L);
	viewG_start();
	return 0;
}
static int lua_viewG_end(lua_State *L) {
	UNUSED(L);
	viewG_end();
	return 0;
}
static int lua_viewG_flush(lua_State *L) {
	UNUSED(L);
	RET_BOOL(viewG_flush());
}
static int lua_viewG_resizeWindow(lua_State *L) {
	RET_BOOL(viewG_resizeWindow(
		luaL_checkint(L, 1),
		luaL_checkint(L, 2)
	));
}
static int lua_viewG_defColor(lua_State *L) {
	RET_BOOL(viewG_defColor(
		luaL_checkint(L, 1),
		(unsigned char) (luaL_checkint(L, 2) & 0xFF),
		(unsigned char) (luaL_checkint(L, 3) & 0xFF),
		(unsigned char) (luaL_checkint(L, 4) & 0xFF)
	));
}
static int lua_viewG_flash(lua_State *L) {
	UNUSED(L);
	viewG_flash();
	return 0;
}
static int lua_viewG_beep(lua_State *L) {
	UNUSED(L);
	viewG_beep();
	return 0;
}

static int lua_view_new(lua_State *L) {
	View v;
	v = view_init(
		lua_newuserdata(L, sizeof(struct View)),
		luaL_checkint(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3),
		luaL_checkint(L, 4));

	if (NULL == v) {
		lua_pushnil(L);
		lua_pushliteral(L, "unable to create " VIEW_MT);
		return 2;
	}
	lua_pushlightuserdata(L, v);
	luaL_getmetatable(L, VIEW_MT);
	lua_setmetatable(L, -2);
	return 1;
}
static int lua_view_delete(lua_State *L) {
	viewG_flash();
	view_delete(check_view(L, 1));
	return 0;
}

static int lua_view_x(lua_State *L) {
	lua_pushinteger(L, view_x(check_view(L, 1)));
	return 1;
}
static int lua_view_y(lua_State *L) {
	lua_pushinteger(L, view_y(check_view(L, 1)));
	return 1;
}
static int lua_view_w(lua_State *L) {
	lua_pushinteger(L, view_w(check_view(L, 1)));
	return 1;
}
static int lua_view_h(lua_State *L) {
	lua_pushinteger(L, view_h(check_view(L, 1)));
	return 1;
}
static int lua_view_move(lua_State *L) {
	RET_BOOL(view_move(check_view(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3)
	));
}
static int lua_view_resize(lua_State *L) {
	RET_BOOL(view_resize(check_view(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3)
	));
}
static Style args2style(lua_State *L) {
	/* MUST be in same order as */
	static const char *const colors[] = {
		"black", "red", "green", "yellow", "blue", "magenta", "cyan", "white", NULL
	};
	static const char *const video[] = {
		"normal", "standout", "underline", "reverse", "blink", "dim", "bold", "altcharset", NULL
	};
	int fg  = FG( luaL_checkoption(L, 2, "black", colors) );
	int bg  = BG( luaL_checkoption(L, 3, "black", colors) );
	int vid =     luaL_checkoption(L, 4, "normal", video) << 16;
	return vid | bg | fg;
}
static int lua_view_setStyle(lua_State *L) {
	RET_BOOL(view_setStyle(check_view(L, 1), args2style(L)));
}
static int lua_view_setBGStyle(lua_State *L) {
	RET_BOOL(view_setBGStyle(check_view(L, 1), args2style(L)));
}
static int lua_view_show(lua_State *L) {
	RET_BOOL(view_show(check_view(L, 1)));
}
static int lua_view_hide(lua_State *L) {
	RET_BOOL(view_hide(check_view(L, 1)));
}
static int lua_view_isHidden(lua_State *L) {
	lua_pushboolean(L, view_isHidden(check_view(L, 1)));
	return 1;
}
static int lua_view_toTop(lua_State *L) {
	view_toTop(check_view(L, 1)); return 0;
}
static int lua_view_toBottom(lua_State *L) {
	view_toBottom(check_view(L, 1)); return 0;
}
static int lua_view_curX(lua_State *L) {
	lua_pushinteger(L, view_curX(check_view(L, 1)));
	return 1;
}
static int lua_view_curY(lua_State *L) {
	lua_pushinteger(L, view_curY(check_view(L, 1)));
	return 1;
}
static int lua_view_getCursor(lua_State *L) {
	lua_pushinteger(L, view_curX(check_view(L, 1)));
	lua_pushinteger(L, view_curY(check_view(L, 1)));
	return 2;
}
static int lua_view_setCursor(lua_State *L) {
	RET_BOOL(view_setCursor(check_view(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3)
	));
}
static int lua_view_moveCursor(lua_State *L) {
	RET_BOOL(view_moveCursor(check_view(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3)
	));
}
static int lua_view_border(lua_State *L) {
	RET_BOOL(view_border(check_view(L, 1)));
}
static int lua_view_vline(lua_State *L) {
	RET_BOOL(view_vline(check_view(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3),
		luaL_checkint(L, 4)
	));
}
static int lua_view_hline(lua_State *L) {
	RET_BOOL(view_hline(check_view(L, 1),
		luaL_checkint(L, 2),
		luaL_checkint(L, 3),
		luaL_checkint(L, 4)
	));
}
static int lua_view_touch(lua_State *L) {
	RET_BOOL(view_touch(check_view(L, 1)));
}
static int lua_view_clear(lua_State *L) {
	RET_BOOL(view_clear(check_view(L, 1)));
}
static int lua_view_clearLine(lua_State *L) {
	RET_BOOL(view_clearLine(check_view(L, 1),
		luaL_checkint(L, 2)
	));
}
static int lua_view_setScroll(lua_State *L) {
	RET_BOOL(view_setScroll(check_view(L, 1), lua_toboolean(L, 2)));
}
static int lua_view_scroll(lua_State *L) {
	RET_BOOL(view_scroll(check_view(L, 1), luaL_checkint(L, 2)));
}
static int lua_view_copy(lua_State *L) {
	RET_BOOL(view_copy(
		check_view(L, 1),
		check_view(L, 2),
		luaL_checkint(L, 3),
		luaL_checkint(L, 4),
		luaL_checkint(L, 5),
		luaL_checkint(L, 6),
		luaL_checkint(L, 7),
		luaL_checkint(L, 8)
	));
}
static int lua_view_getc(lua_State *L) {
	char chr = view_getc(check_view(L, 1));
	lua_pushlstring(L, &chr, 1);
	return 1;
}
static int lua_view_gets(lua_State *L) {
	size_t len = (size_t) luaL_checkint(L, 2);
	char str[1024];
	if (len <= 0 || len > 1024) return luaL_error(L, "len out of bounds: (0, 1024)");
	view_gets(check_view(L, 1), str, len);
	lua_pushstring(L, str);
	return 1;
}
static int lua_view_insc(lua_State *L) {
	char  	chr;
	size_t	size;
	chr = *luaL_checklstring(L, 2, &size);
	RET_BOOL(view_insc(check_view(L, 1), chr));
}
static int lua_view_putc(lua_State *L) {
	char  	chr;
	size_t	size;
	chr = *luaL_checklstring(L, 2, &size);
	RET_BOOL(view_putc(check_view(L, 1), chr));
}
static int lua_view_puts(lua_State *L) {
	RET_BOOL(view_puts(check_view(L, 1), luaL_checkstring(L, 2)));
}
static int lua_view_printf(lua_State *L) {
	luaL_checkstring(L, 2);
	lua_getfield(L, 2, "format");
	lua_insert(L, 2);
	lua_call(L, lua_gettop(L)-2, 1);
	RET_BOOL(view_puts(check_view(L, 1), lua_tostring(L, -1)));
}
static int lua_view__tostring(lua_State *L) {
	char buffer[32];
	sprintf(buffer, VIEW_MT ": %p", (void*) check_view(L, 1));
	lua_pushstring(L, buffer);
	return 1;
}

static const luaL_Reg lua_view_mt[] = {
/*	{  "delete",    	lua_view_delete    	}, */
  	{  "x",         	lua_view_x         	},
  	{  "y",         	lua_view_y         	},
  	{  "w",         	lua_view_w         	},
  	{  "h",         	lua_view_h         	},
  	{  "move",      	lua_view_move      	},
  	{  "resize",    	lua_view_resize    	},
  	{  "setStyle",  	lua_view_setStyle  	},
  	{  "setBGStyle",	lua_view_setBGStyle	},
  	{  "show",      	lua_view_show      	},
  	{  "hide",      	lua_view_hide      	},
  	{  "isHidden",  	lua_view_isHidden  	},
  	{  "toTop",     	lua_view_toTop     	},
  	{  "toBottom",  	lua_view_toBottom  	},
  	{  "curX",      	lua_view_curX      	},
  	{  "curY",      	lua_view_curY      	},
  	{  "getCursor", 	lua_view_getCursor 	},
  	{  "setCursor", 	lua_view_setCursor 	},
  	{  "moveCursor",	lua_view_moveCursor	},
  	{  "border",    	lua_view_border    	},
  	{  "vline",     	lua_view_vline     	},
  	{  "hline",     	lua_view_hline     	},
  	{  "touch",     	lua_view_touch     	},
  	{  "clear",     	lua_view_clear     	},
  	{  "clearLine", 	lua_view_clearLine 	},
  	{  "setScroll", 	lua_view_setScroll 	},
  	{  "scroll",    	lua_view_scroll    	},
  	{  "copy",      	lua_view_copy      	},
  	{  "getc",      	lua_view_getc      	},
  	{  "gets",      	lua_view_gets      	},
/*	{  "scanf",     	lua_view_scanf     	}, */
  	{  "insc",      	lua_view_insc      	},
  	{  "putc",      	lua_view_putc      	},
  	{  "puts",      	lua_view_puts      	},
  	{  "printf",    	lua_view_printf    	},
{NULL,NULL}
};

static const luaL_Reg lua_view_lib[] = {
	{  "new",         	lua_view_new          	},
	{  "beep",        	lua_viewG_beep        	},
	{  "flash",       	lua_viewG_flash       	},
	{  "flush",       	lua_viewG_flush       	},
	{  "defColor",    	lua_viewG_defColor    	},
	{  "resizeWindow",	lua_viewG_resizeWindow	},
	{  "start",       	lua_viewG_start       	},
	{  "finish",      	lua_viewG_end         	}, /* RENAMED!!! */
{NULL,NULL}
};

#if LUA_VERSION_NUM == 501
int luaopen_termview(lua_State *L) {

	/* Setup auto close:
	** we create a proxy (full)userdata that
	** resets the terminal when it's GC */
	void* libproxy = lua_newuserdata(L, 0);

	if (libproxy == NULL) {
		lua_pushstring(L, "error creating lua proxy");
		lua_error(L); /* never returns */
	}

	/* Proxy metatable */
	lua_newtable(L);
	lua_pushcfunction(L, termview__gc);
	lua_setfield(L, -2, "__gc");

	/* Library table */
	lua_newtable(L);
	/* Register methods */
	luaL_register(L, NULL, lua_view_lib);
	/* Push stdview */
	lua_pushlightuserdata(L, stdview);

	/* Before adding the stdview to the lib,
	** we need to setup the TermView metatable */
	luaL_newmetatable(L, VIEW_MT);

	lua_newtable(L);
	luaL_register(L, NULL, lua_view_mt);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, lua_view_delete);
	lua_setfield(L, -2, "__gc");

	lua_pushcfunction(L, lua_view__tostring);
	lua_setfield(L, -2, "__tostring");

	/* Now we set the metatable and add it */
	lua_setmetatable(L, -2);   	/* pops metatable */
	lua_setfield(L, -2, "std");	/* pops stdview   */

	/* The table with library functions is on the top
	** of the stack. We set it as the __index of the proxy */
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);

	/* Finally: start the view system & return the proxy */
	/* viewG_start(); */
	return 1;
}

/* Codigo original de carga (Lua 5.2) */
#elif LUA_VERSION_NUM == 502
int luaopen_termview(lua_State *L){
	luaL_newlib(L, lua_view_lib);
	lua_pushlightuserdata(L, stdview);

	luaL_newmetatable(L, VIEW_MT);

	lua_newtable(L);
	luaL_setfuncs(L, lua_view_mt, 0);
	lua_setfield(L, -2, "__index");

	lua_pushcfunction(L, lua_view_delete);
	lua_setfield(L, -2, "__gc");

	lua_pushcfunction(L, lua_view__tostring);
	lua_setfield(L, -2, "__tostring");

	lua_setmetatable(L, -2);
	lua_setfield(L, -2, "std");
	return 1;
}
#else
#define XSTR(x) STR(x)
#define STR(x)	#x
#pragma message "Unknown Lua version: " XSTR(LUA_VERSION_NUM)
#error Unknown Lua version
#endif
