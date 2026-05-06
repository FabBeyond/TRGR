#pragma once
#include <sol/sol.hpp>

extern sol::state lua;

ZLayer intToZLayer(int value);
void setupLuaAPI();
void runScript(const std::string& code);