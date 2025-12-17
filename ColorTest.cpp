#include <iostream>
#include <string>
#include <windows.h>
#include <cstdio> // For std::wprintf, if needed

// #ifdef _WIN32
// #include <io.h>
// #include <fcntl.h>
// #endif

// void setupConsole() {
//     #ifdef _WIN32
//     // Set the console output code page to UTF-8 (65001)
//     SetConsoleOutputCP(65001); 
//     // This function is defined in <windows.h> which is often included by other headers.
//     // If not, you might need to manually include <windows.h>
//     #endif
// }

int main() {
    //setupConsole();
    std::string color = "\033[30m♣\033[0m";     //30 black, 
    std::cout << color << "dd" << std::endl;
}