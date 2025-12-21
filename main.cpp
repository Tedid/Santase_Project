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
 * This project is an emulator to the familiar card game for two - Santase.
 *
 */

#include <iostream>
#include <vector>
// #include <fstream>
#include <algorithm>
#include <random>
#include <windows.h>
#include <string>

#include "utils.hpp"
#include "gameFunctions.hpp"

constexpr int DEFAULT_REQUIRED_POINTS_TO_WIN = 11;
constexpr int DEFAULT_NON_TRUMP_MARRIAGE = 20;
constexpr int DEFAULT_TRUMP_MARRIAGE = 40;
constexpr bool DEFAULT_ARE_POINTS_VISIBLE = true;
constexpr bool DEFAULT_LAST_TRICK_BONUS = true;

constexpr int SUIT_MAX_LENGTH = 4;  //UTF-8 (3 bytes) + "\0"
constexpr int RANK_MAX_LENGTH = 3;  //"10" + "\0" at most

constexpr int DECK_MAX_SIZE = 24;
constexpr int HAND_MAX_SIZE = 6;
constexpr int THROWN_CARDS_MAX_NUMBER = 2;


int main()
{
    setupConsole(); // Ensures proper display of suit symbols (♣,♠,♥,♦)

    bool hasGameStarted = false;
    bool wereSettingsModified = false;
    char trumpSuit[SUIT_MAX_LENGTH]; // is one of ("♣","♠","♥","♦")
    Card deck[DECK_MAX_SIZE];
    Card P1Hand[HAND_MAX_SIZE], P2Hand[HAND_MAX_SIZE];
    Card thrownCards[THROWN_CARDS_MAX_NUMBER];
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
                thrownCards.push_back(P1Hand.at(index));
                std::cout << "P1 played " << P1Hand.at(index) << std::endl;
                P1Hand.erase(P1Hand.begin() + index); // Removing the played card from the hand
            }
            else
            {
                thrownCards.push_back(P2Hand.at(index));
                std::cout << "P2 played " << P2Hand.at(index) << std::endl;
                P2Hand.erase(P2Hand.begin() + index); // Removing the played card from the hand
            }

            // If a player puts the first hand on the "table":
            if (thrownCards.size() == 1)
            {
                firstPlayedPlayerId = currentPlayerId;
            }
            else if (thrownCards.size() == 2) // If both players have played a card on the "table"
            {
                bool P1WinsTrick;

                if (isTrump(thrownCards.front(), trumpSuit) && !isTrump(thrownCards.back(), trumpSuit))
                {                                             // only first card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 1); // when first player plays first, he gives a trump
                }
                else if (!isTrump(thrownCards.back(), trumpSuit) && isTrump(thrownCards.front(), trumpSuit))
                {                                             // only second card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 1); // when second player played first, then first player played trump
                }
                else if (isTrump(thrownCards.back(), trumpSuit) && isTrump(thrownCards.front(), trumpSuit))
                {                                                                        // Both cards are trump
                    P1WinsTrick = compareCards(thrownCards.front(), thrownCards.back()); // Stronger card wins
                }
                else
                { // Both cards are non-trump
                    std::string dominantSuit = getSuit(firstPlayedPlayerId == 1 ? thrownCards.front() : thrownCards.back());
                    // Dominant suit is the first played card's suit

                    if (getSuit(thrownCards.back()) != dominantSuit)
                    { // If second card's suit doesn't match the first one
                        P1WinsTrick = (firstPlayedPlayerId == 1);
                        // If P1 played first, he wins the trick
                    }
                    else
                    {
                        P1WinsTrick = compareCards(thrownCards.front(), thrownCards.back()); // Stronger card wins
                    }
                }

                // CLOSED CHECK NEEDS TO BE ADDED!
                std::cout << (P1WinsTrick ? "P1" : "P2") << " wins the trick! ";

                thrownCards.clear();
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
