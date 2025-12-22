#include "utils.hpp"

void setupConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

bool isDigit(const char *str) {
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
    std::cout << prompt << " [y/n]: " << std::endl;
    char *answer;

    while (true)
    {
        std::cin >> answer;

        int answerLen = 0;
        char *pointer = answer;
        while(*pointer != '\0'){
            answerLen++;
        }
        if (answerLen == 1)
        {
            char c = std::tolower(answer[0]);

            if (c == 'n')
            {
                return false;
            }
            else if (c == 'y')
            {
                return true;
            }
        }

        // In case answer is invalid:
        std::cout << "Invalid answer, please try again. [y/n]: " << std::endl;
    }
}
