cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(TRGR VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES}
    src/luaAPI.cpp
    src/luaAPI.hpp
    src/scriptPopup.cpp
    src/scriptPopup.hpp
    src/createPopup.cpp
    src/createPopup.hpp)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

include(FetchContent)

FetchContent_Declare(
    lua
    GIT_REPOSITORY "https://github.com/walterschell/Lua.git"
    GIT_TAG        v5.4.7
)
FetchContent_MakeAvailable(lua)

if(TARGET lua_static)
    if(ANDROID OR IOS)
        target_compile_definitions(lua_static PRIVATE LUA_USE_POSIX=0)
    endif()
endif()

FetchContent_Declare(
    sol2
    GIT_REPOSITORY "https://github.com/ThePhD/sol2.git"
    GIT_TAG        c1f95a7
)
FetchContent_MakeAvailable(sol2)

target_link_libraries(${PROJECT_NAME} lua_static sol2)
target_compile_definitions(${PROJECT_NAME} PUBLIC SOL_OPTIONAL_IMPLEMENTATION=0)

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})