#include <iostream>
#include <vector>
// #include <fstream>
#include <algorithm>
#include <random>
#include <windows.h>
#include <string>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

const std::string COLOR_RED = "\033[31m";
const std::string COLOR_BLACK = "\033[30m";
const std::string COLOR_RESET = "\033[0m";

const std::string HEARTS = COLOR_RED + "♥" + COLOR_RESET;
const std::string DIAMONDS = COLOR_RED + "♦" + COLOR_RESET;
const std::string CLUBS = COLOR_BLACK + "♣" + COLOR_RESET;
const std::string SPADES = COLOR_BLACK + "♠" + COLOR_RESET;

// Sorting priorities:----------------------
const std::string SUIT_CHARS = "♠♥♦♣";
const int SUIT_VALUES[] = {4, 3, 2, 1};
// ♠=4, ♥=3, ♦=2, ♣=1

const std::string RANK_CHARS = "A1KQJ9"; // "1" represents "10"
const int RANK_VALUES[] = {6, 5, 4, 3, 2, 1};
// A=6, 10=5, K=4, Q=3, J=2, 9=1
//-----------------------------------------

const int DEFAULT_REQUIRED_POINTS_TO_WIN = 11;
const int DEFAULT_NON_TRUMP_MARRIAGE = 20;
const int DEFAULT_TRUMP_MARRIAGE = 40;
const bool DEFAULT_ARE_POINTS_VISIBLE = true;
const bool DEFAULT_LAST_TRICK_BONUS = true;

void setupConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); // Set the console to UTF-8 (65001)
#endif
}

bool isDigit(const std::string &str);
// replaced char-based isTrump with string-based version
bool isTrump(const std::string &card, const std::string &trumpSuit)
{
    if (trumpSuit.empty())
        return false;
    return card.back() == trumpSuit.back();
}
bool getConfirmation(const std::string &prompt);

void initializeDeck(std::vector<std::string> &deck);
void dealCards(std::vector<std::string> &deck, std::vector<std::string> &P1, std::vector<std::string> &P2);
void revealTrump(std::vector<std::string> &deck, std::string &trumpSuit);
int getSuitValue(const std::string &card);
int getRankValue(const std::string &card);
bool compareCards(const std::string &card1, const std::string &card2);

std::string generateRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage);
std::string getSuit(std::string card);
std::string playerHand(const std::vector<std::string> &hand);

int main()
{
    setupConsole(); // Ensures proper display of suit symbols (♣,♠,♥,♦)

    bool hasGameStarted = false;
    bool wereSettingsModified = false;
    std::string trumpSuit; // is one of ("♣","♠","♥","♦") stored as a string
    std::vector<std::string> deck;
    std::vector<std::string> P1Hand, P2Hand;
    std::vector<std::string> tableCards;
    int firstPlayedPlayerId = 0;
    int P1GamePoints = 0, P2Gamepoints = 0;
    int P1RoundPoints = 0, P2RoundPoints = 0;

    int currentPlayerId = 1; // 1 for P1, 2 for P2
    int lastRoundWonPlayerId = 1;

    //---- Editable through settings ----
    int requiredPointsToWin = DEFAULT_REQUIRED_POINTS_TO_WIN;
    int nonTrumpMarriage = DEFAULT_NON_TRUMP_MARRIAGE;
    int trumpMarriage = DEFAULT_TRUMP_MARRIAGE;
    bool arePointsVisible = DEFAULT_ARE_POINTS_VISIBLE;
    bool lastTrickBonus = DEFAULT_LAST_TRICK_BONUS;
    //-----------------------------------

    bool gameOver = false;
    while (!gameOver) // round start
    {

        if (!hasGameStarted)
        {
            std::cout << "Type 'start' to begin a new game" << std::endl;
        }
        else
        {
            std::cout << "P" << currentPlayerId << "'s turn:" << std::endl;
            std::cout << "Hand: " << playerHand((currentPlayerId == 1 ? P1Hand : P2Hand)) << std::endl;
            std::cout << "Trump suit: " << trumpSuit << std::endl;
            std::cout << "Bottom card: " << deck.front() << std::endl;
            std::cout << "Cards left in deck: " << deck.size() << std::endl;
            std::cout << std::endl;
        }

        std::cout << ">";

        std::string fullComm;
        std::getline(std::cin, fullComm);

        // Useful for two-word commands later:
        std::string firstCommWord = fullComm.substr(0, fullComm.find_first_of(' '));

        // A simple line of code to convert the command to lowerCase:
        std::transform(firstCommWord.begin(), firstCommWord.end(), firstCommWord.begin(), [](unsigned char c)
                       { return std::tolower(c); });

        if (firstCommWord == "start")
        {

            // In order to not reset the game on mistake:
            if (hasGameStarted)
            {
                std::string prompt = "This will reset this whole game and start it from scratch. Continue?";
                if (!getConfirmation(prompt))
                {
                    continue; // Dismiss the command
                }
            }

            if (wereSettingsModified)
            {
                std::string prompt = "Do you want to reset the settings to their defaults?";
                if (getConfirmation(prompt))
                {
                    requiredPointsToWin = DEFAULT_REQUIRED_POINTS_TO_WIN;
                    nonTrumpMarriage = DEFAULT_NON_TRUMP_MARRIAGE;
                    trumpMarriage = DEFAULT_TRUMP_MARRIAGE;
                    arePointsVisible = DEFAULT_ARE_POINTS_VISIBLE;
                    lastTrickBonus = DEFAULT_LAST_TRICK_BONUS;

                    wereSettingsModified = false;
                }
            }

            P1GamePoints = 0, P2Gamepoints = 0;
            P1RoundPoints = 0, P2RoundPoints = 0;
            hasGameStarted = true;

            initializeDeck(deck); // Shuffling deck
            dealCards(deck, P1Hand, P2Hand);
            revealTrump(deck, trumpSuit); // Top card goes under and becomes a trump
        }
        else if (firstCommWord == "rules")
        {
            std::string ruleset = generateRulesString(requiredPointsToWin, nonTrumpMarriage, trumpMarriage);
            std::cout << ruleset << std::endl;
        }
        else if (firstCommWord == "settings")
        {
            while (true)
            {
                std::cout << "--- SANTASE (66) SETTINGS ---" << std::endl;
                std::cout << "1) Target points to win [" << requiredPointsToWin << "]" << std::endl;
                std::cout << "2) Marriage points (non-trump/trump) [" << nonTrumpMarriage << "/" << trumpMarriage << "]" << std::endl;
                std::cout << "3) Show players' points [" << (arePointsVisible ? "on" : "off") << "]" << std::endl;
                std::cout << "4) Last trick +10 [" << (lastTrickBonus ? "on" : "off") << "]" << std::endl;
                std::cout << "Enter number to change or 'back' to apply and return: ";

                std::string comm;

                std::getline(std::cin, comm);

                // Transforming comm toLower
                std::transform(comm.begin(), comm.end(), comm.begin(), [](unsigned char c)
                               { return std::tolower(c); });

                if (comm == "1")
                {
                    while (true)
                    {
                        std::cout << "Enter a points target for the game: ";
                        std::string targetUpdateLine;
                        std::getline(std::cin, targetUpdateLine);

                        if (isDigit(targetUpdateLine))
                        {
                            requiredPointsToWin = std::stoi(targetUpdateLine);
                            std::cout << "Points target changed to " << requiredPointsToWin << "." << std::endl;
                            break;
                        }
                        else
                        {
                            std::cout << "Invalid input. Try again" << std::endl;
                        }
                    }

                    wereSettingsModified = true;
                }
                else if (comm == "2")
                {
                    std::string line;

                    while (true) // Non-trump marriage
                    {
                        std::cout << "Enter points for the non-trump marriage: ";
                        std::getline(std::cin, line);

                        if (isDigit(line))
                        {
                            nonTrumpMarriage = std::stoi(line);
                            break;
                        }
                        else
                        {
                            std::cout << "Invalid input. Try again" << std::endl;
                        }
                    }

                    while (true) // Trump marriage
                    {
                        std::cout << "Enter points for the trump marriage: ";
                        std::getline(std::cin, line);

                        if (isDigit(line))
                        {
                            trumpMarriage = std::stoi(line);
                            break;
                        }
                        else
                        {
                            std::cout << "Invalid input. Try again" << std::endl;
                        }
                    }

                    std::cout << "Marriage points now (non-trump/trump) [" << nonTrumpMarriage << "/" << trumpMarriage << "]" << std::endl;
                }
                else if (comm == "3")
                {
                    if (arePointsVisible)
                    {
                        std::string prompt = "Do you want to stop seeing players' points?";
                        if (getConfirmation(prompt))
                        {
                            arePointsVisible = false;
                        }
                    }
                    else
                    {
                        std::string prompt = "Do you want to start seeing players' points?";
                        if (getConfirmation(prompt))
                        {
                            arePointsVisible = true;
                        }
                    }
                }
                else if (comm == "4")
                {
                    if (lastTrickBonus)
                    {
                        std::string prompt = "Do you want to stop the 10 bonus points for the last trick?";
                        if (getConfirmation(prompt))
                        {
                            lastTrickBonus = false;
                        }
                    }
                    else
                    {
                        std::string prompt = "Do you want to have 10 bonus points for the last trick?";
                        if (getConfirmation(prompt))
                        {
                            lastTrickBonus = true;
                        }
                    }
                }
                else if (comm == "back")
                {
                    std::cout << "Settings applied!" << std::endl;

                    if (wereSettingsModified)
                    {
                        // Check if a player hasn't already won with updated required points
                        if (P1GamePoints >= requiredPointsToWin || P2Gamepoints >= requiredPointsToWin)
                        { // TODO: somehow end the game
                        }
                    }

                    break;
                }

                std::cout << "Invalid command. Please try again." << std::endl;
            }
        }
        else if (firstCommWord == "hand")
        {
            std::vector<std::string> currentHand;

            if (currentPlayerId == 1)
            {
                currentHand = P1Hand;
            }
            else
            {
                currentHand = P2Hand;
            }

            std::cout << "Your hand (P" << currentPlayerId << "): " << playerHand(currentHand) << std::endl;
        }
        else if (firstCommWord == "play")
        {
            std::string input;

            while (true)
            {
                std::getline(std::cin, input);
                if (isDigit(input) && std::stoi(input) >= 0 && std::stoi(input) <= 5)
                    break;
                std::cout << "Invalid hand index. Enter a number from 0 to 5: ";
            }

            int index = std::stoi(input);

            if (currentPlayerId == 1)
            {
                tableCards.push_back(P1Hand.at(index));
                std::cout << "P1 played " << P1Hand.at(index) << std::endl;
                P1Hand.erase(P1Hand.begin() + index); // Removing the played card from the hand
            }
            else
            {
                tableCards.push_back(P2Hand.at(index));
                std::cout << "P2 played " << P2Hand.at(index) << std::endl;
                P2Hand.erase(P2Hand.begin() + index); // Removing the played card from the hand
            }

            // If a player puts the first hand on the "table":
            if (tableCards.size() == 1)
            {
                firstPlayedPlayerId = currentPlayerId;
            }
            else if (tableCards.size() == 2) // If both players have played a card on the "table"
            {
                bool P1WinsTrick;

                if (isTrump(tableCards.front(), trumpSuit) && !isTrump(tableCards.back(), trumpSuit))
                {                                             // only first card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 1); // when first player plays first, he gives a trump
                }
                else if (!isTrump(tableCards.back(), trumpSuit) && isTrump(tableCards.front(), trumpSuit))
                {                                             // only second card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 1); // when second player played first, then first player played trump
                }
                else if (isTrump(tableCards.back(), trumpSuit) && isTrump(tableCards.front(), trumpSuit))
                {                                                                      // Both cards are trump
                    P1WinsTrick = compareCards(tableCards.front(), tableCards.back()); // Stronger card wins
                }
                else
                { // Both cards are non-trump
                    std::string dominantSuit = getSuit(firstPlayedPlayerId == 1 ? tableCards.front() : tableCards.back());
                    // Dominant suit is the first played card's suit

                    if (getSuit(tableCards.back()) != dominantSuit)
                    { // If second card's suit doesn't match the first one
                        P1WinsTrick = (firstPlayedPlayerId == 1);
                        // If P1 played first, he wins the trick
                    }
                    else
                    {
                        P1WinsTrick = compareCards(tableCards.front(), tableCards.back()); // Stronger card wins
                    }
                }

                // CLOSED CHECK NEEDS TO BE ADDED!
                std::cout << (P1WinsTrick ? "P1" : "P2") << " wins the trick! ";

                tableCards.clear();
            }

            currentPlayerId = 3 - currentPlayerId; // Looping the current player's turn
        }
        else if (firstCommWord == "switch-nine")
        {
        }
        else if (firstCommWord == "marriage")
        {
        }
        else if (firstCommWord == "close")
        {
        }
        else if (firstCommWord == "last-trick")
        {
        }
        else if (firstCommWord == "trump")
        {
        }
        else if (firstCommWord == "history")
        {
        }
        else if (firstCommWord == "status")
        {
        }
        else if (firstCommWord == "stop")
        {
        }
        else if (firstCommWord == "surrender")
        {
        }
        else if (firstCommWord == "surrender-forever")
        {
        }
        else if (firstCommWord == "save")
        {
        }
        else if (firstCommWord == "load")
        {
        }
        else
        {
            // std::cout << "\033[2J\033[H";
            std::cout << "Unknown command. Please try again." << std::endl;
            continue;
        }

        std::cout << std::endl;

        // bool roundOver = false;

        // while (!roundOver) // Hand start
        // {
        // }
    }
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

void initializeDeck(std::vector<std::string> &deck)
{
    deck.clear();

    for (int j = 0; j <= 3; j++)
    {
        std::string suit;
        switch (j)
        {
        case 0:
            suit = CLUBS;
            break;
        case 1:
            suit = DIAMONDS;
            break;
        case 2:
            suit = HEARTS;
            break;
        case 3:
            suit = SPADES;
            break;
        }

        for (int i = 0; i <= 5; i++)
        {
            switch (i)
            {
            case 0:
                deck.push_back("9" + suit);
                break;
            case 1:
                deck.push_back("J" + suit);
                break;
            case 2:
                deck.push_back("Q" + suit);
                break;
            case 3:
                deck.push_back("K" + suit);
                break;
            case 4:
                deck.push_back("10" + suit);
                break;
            case 5:
                deck.push_back("A" + suit);
                break;
            }
        }
    }

    // SHUFFLING DECK
    std::shuffle(deck.begin(), deck.end(), std::mt19937{std::random_device{}()});
}

void dealCards(std::vector<std::string> &deck, std::vector<std::string> &P1, std::vector<std::string> &P2)
{
    P1.clear();
    P2.clear();

    for (int j = 0; j <= 1; j++)
    {
        for (int i = 0; i <= 2; i++)
        {
            std::string card = deck.back();
            deck.pop_back();
            P1.push_back(card);
        }
        for (int i = 0; i <= 2; i++)
        {
            std::string card = deck.back();
            deck.pop_back();
            P2.push_back(card);
        }
    }

    // Sorting hands:
    std::sort(P1.begin(), P1.end(), compareCards);
    std::sort(P2.begin(), P2.end(), compareCards);
}

void revealTrump(std::vector<std::string> &deck, std::string &trumpSuit)
{
    std::string topCard = deck.back();
    deck.pop_back();
    deck.insert(deck.begin(), topCard); // The trump card is now below the deck

    trumpSuit = getSuit(topCard);
}

int getSuitValue(const std::string &card)
{
    std::string suit = getSuit(card);

    // Finding the actual suit character (♠,♥,♦,♣) within the colored string
    int pos = suit.find_first_of(SUIT_CHARS);

    // Extracting the actual suit (all suit characters are 3-byte UTF-8)
    std::string noColorSuit = suit.substr(pos, 3);

    int pointer = SUIT_CHARS.find(noColorSuit);

    // Divide by 3 to get the character index for SUIT_VALUES array
    return SUIT_VALUES[pointer / 3];

    return -1; // Return -1 if suit character is not found
}

int getRankValue(const std::string &card)
{
    char rank = card.front();
    int pointer = RANK_CHARS.find(rank);

    if (pointer <= 5)
        return RANK_VALUES[pointer];

    return -1;
}

bool compareCards(const std::string &card1, const std::string &card2)
{
    // Primary sort by suit
    int suit1 = getSuitValue(card1);
    int suit2 = getSuitValue(card2);

    if (suit1 > suit2)
        return true;
    else if (suit1 < suit2)
        return false;

    // If suits are equal, sort by rank
    int rank1 = getRankValue(card1);
    int rank2 = getRankValue(card2);

    return rank1 > rank2;
}

std::string generateRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage)
{

    std::string rules_string = "------------ SANTASE GAME RULES ------------\n";
    rules_string += "* Each player gets 6 cards. The trump suit is the suit of the next card in the deck.\n";
    rules_string += "This card gets put below the deck.\n";
    rules_string += "* Card values: A=11, 10=10, K=4, Q=3, J=2, 9=0.\n";
    rules_string += "* A marriage (K+Q of the same suit) gives " + std::to_string(nonTrumpMarriage) + " points, or " + std::to_string(trumpMarriage) + " if of the trump suit.\n";
    rules_string += "* The winner of a trick leads next and draws a card from the stock.\n";
    rules_string += "* Players must follow suit and trump only after the stock has been closed.\n";
    rules_string += "* The first player to reach 66 points wins the round.\n";
    rules_string += "* The game finishes when a player hits " + std::to_string(requiredPointsToWin) + " match points.\n";

    return rules_string;
}

std::string getSuit(std::string card)
{
    if (card.front() == '1')
    {
        return card.substr(2, card.size() - 2);
    }
    else
    {
        return card.substr(1, card.size() - 1);
    }
}

std::string playerHand(const std::vector<std::string> &hand)
{
    std::string str = "[ ";

    for (int i = 0; i < hand.size(); i++)
    {
        str += hand[i];

        if (i != hand.size() - 1)
            str += ", ";
    }
    str += " ]";

    return str;
}
