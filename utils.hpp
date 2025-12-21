#pragma once

#include <iostream>
#include <windows.h>

void setupConsole();

void setColor(int color);

bool isDigit(const std::string &str);
// replaced char-based isTrump with string-based version
bool getConfirmation(const std::string &prompt);
