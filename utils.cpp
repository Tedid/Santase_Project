#include "utils.hpp"

void setupConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
}

bool isDigit(const std::string &str)
{
    if (str.empty())
    {
        return false;
    }

    for (int i = 0; i < str.length(); i++)
    {
        if (!std::isdigit(static_cast<unsigned char>(str[i])))
        {
            return false; // Found a non-digit character
        }
    }

    return true;
}

bool getConfirmation(const std::string &prompt)
{
    std::cout << prompt << " [y/n]: " << std::endl;
    std::string answer;

    while (true)
    {
        std::getline(std::cin, answer);

        if (answer.length() == 1)
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
