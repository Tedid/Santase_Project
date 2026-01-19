/**
 *
 * Solution to course project # 2
 * Introduction to programming course
 * Faculty of Mathematics and Informatics of Sofia University
 * Winter semester 2025/2026
 *
 * @author Teodor Genov
 * @idnumber 3MI0600648
 * @compiler GCC
 *
 * Header file containing function declarations for useful methods used in the project.
 *
 */

#pragma once

#include <windows.h>
#include <iostream>
#include "cardStruct.hpp"

constexpr size_t MAX_STR_LEN = 1024;

void setupConsole();

void setColor(int color);

bool isDigit(const char* str);
bool getConfirmation(const char* prompt);
