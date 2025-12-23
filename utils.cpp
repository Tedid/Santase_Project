#include "utils.hpp"

void setupConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

bool isDigit(const char *str)
{
    if (!str) return false;
    const char *pointer = str;
    while (*pointer != '\0') { 
        if(!std::isdigit(*pointer)){
            return false;
        }
        pointer++;
    }
    return true;
}

bool getConfirmation(const char *prompt)
{
    std::cout << prompt << " [y/n]: ";
    char answerChar;

    while (true)
    {
        std::cin >> answerChar; // Read a single character

        // Directly discard characters until newline or EOF
        while (std::cin.get() != '\n');

        char c = std::tolower(answerChar);

        if (c == 'n')
        {
            return false;
        }
        else if (c == 'y')
        {
            return true;
        }
        // In case answer is invalid:
        std::cout << "Invalid answer, please try again. [y/n]: ";
    }
}
