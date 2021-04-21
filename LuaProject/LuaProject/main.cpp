
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


int n = 0;

int func(lua_State *L) 
{
	printf("func_top=%d top=%s\n", lua_gettop(L), lua_tostring(L, -1));
	if (!n) {
		++n;
		int result = lua_yield(L, 1);
		printf("continue running  %d\n", result);
		return 2;
	}
	else {
		return 1;
	}
}

int main(int argc, char* const argv[]) {
	lua_State *L = luaL_newstate();

	/* init lua library */
	lua_pushcfunction(L, luaopen_base);
	if (lua_pcall(L, 0, 0, 0) != 0) {
		return 1;
	}
	lua_pushcfunction(L, luaopen_package);
	if (lua_pcall(L, 0, 0, 0) != 0) {
		return 2;
	}

	/* create the coroutine */
	lua_State *LL = lua_newthread(L);

	lua_pushcfunction(LL, func);
	lua_pushstring(LL, "hello world");

	/* first time resume */
	if (lua_resume(LL, 1) == LUA_YIELD) {
		printf("first_top=%d top_string=%s\n", lua_gettop(LL), lua_tostring(LL, -1));
		/* twice resume */
		if (lua_resume(LL, 1) == 0) {
			printf("second_top=%d top_string=%s\n", lua_gettop(LL), lua_tostring(LL, -1));
		}
	}

	lua_close(L);

	return 0;
}