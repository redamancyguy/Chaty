#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/timeb.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

lua_State *lua;

int main() {
    luaL_openlibs(lua);
    return 0;
}