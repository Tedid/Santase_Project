#pragma once

#include <iostream>
#include <windows.h>

#include "cardStruct.hpp"

constexpr size_t MAX_STR_LEN = 1024;

void setupConsole();

void setColor(int color);

bool isDigit(const char *str);
bool getConfirmation(const char *prompt);
