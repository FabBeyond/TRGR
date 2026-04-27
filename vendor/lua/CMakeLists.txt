cmake_minimum_required(VERSION 3.21)
project(lua C)

file(GLOB LUA_SOURCES "src/*.c")
list(REMOVE_ITEM LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/lua.c")
list(REMOVE_ITEM LUA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/luac.c")

add_library(lua_static STATIC ${LUA_SOURCES})
target_include_directories(lua_static PUBLIC src)