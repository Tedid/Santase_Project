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
#include <cctype>
#include <cstring>

#include "utils.hpp"
#include "gameFunctions.hpp"
#include "cardStruct.hpp"

constexpr int DEFAULT_REQUIRED_POINTS_TO_WIN = 11;
constexpr int DEFAULT_NON_TRUMP_MARRIAGE = 20;
constexpr int DEFAULT_TRUMP_MARRIAGE = 40;
constexpr bool DEFAULT_ARE_POINTS_VISIBLE = true;
constexpr bool DEFAULT_LAST_TRICK_BONUS = true;

// constexpr size_t MAX_STR_LEN = 1024;

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
    int deckSize = 0;

    Card P1Hand[HAND_MAX_SIZE], P2Hand[HAND_MAX_SIZE];
    int P1HandSize = 0, P2HandSize = 0;

    Card thrownCards[THROWN_CARDS_MAX_NUMBER];
    int thrownCount = 0;

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
            if (currentPlayerId == 1)
            {
                std::cout << "Hand: ";
                printPlayerHand(P1Hand, P1HandSize);
                std::cout << std::endl;
            }

            else
            {
                std::cout << "Hand: ";
                printPlayerHand(P2Hand, P2HandSize);
                std::cout << std::endl;
            }

            std::cout << "Trump suit: ";
            printSuitColored(trumpSuit);
            std::cout << std::endl;
            if (deckSize > 0)
                std::cout << "Bottom card: ";
            cardPrint(deck[0]);
            std::cout << std::endl;
            std::cout << "Cards left in deck: " << deckSize << std::endl;
            std::cout << std::endl;
        }

        std::cout << ">";

        char fullComm[MAX_STR_LEN];
        std::cin.getline(fullComm, MAX_STR_LEN); // Read the entire line

        // Useful for two-word commands later
        char firstCommWord[MAX_STR_LEN];
        // Find the first space or end of string
        size_t spacePos = 0;
        while (fullComm[spacePos] != '\0' && fullComm[spacePos] != ' ')
        {
            spacePos++;
        }
        strncpy(firstCommWord, fullComm, spacePos);
        firstCommWord[spacePos] = '\0';

        // lower case
        for (int i = 0; i < spacePos; i++)
        {
            firstCommWord[i] = std::tolower(firstCommWord[i]);
        }

        if (strcmp(firstCommWord, "start") == 0)
        {

            // In order to not reset the game on mistake:
            if (hasGameStarted)
            {
                const char *prompt = "This will reset this whole game and start it from scratch. Continue?";
                if (!getConfirmation(prompt))
                {
                    continue; // Dismiss the command
                }
            }

            if (wereSettingsModified)
            {
                const char *prompt = "Do you want to reset the settings to their defaults?";
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

            initializeDeck(deck, deckSize); // Shuffling deck
            dealCards(deck, P1Hand, P2Hand, deckSize);
            P1HandSize = HAND_MAX_SIZE;
            P2HandSize = HAND_MAX_SIZE;
            revealTrump(deck, trumpSuit); // Top card goes under and becomes a trump
        }
        else if (strcmp(firstCommWord, "rules") == 0)
        {
            printRulesString(requiredPointsToWin, nonTrumpMarriage, trumpMarriage);
            std::cout << std::endl;
        }
        else if (strcmp(firstCommWord, "settings") == 0)
        {
            while (true)
            {
                std::cout << "--- SANTASE (66) SETTINGS ---" << std::endl;
                std::cout << "1) Target points to win [" << requiredPointsToWin << "]" << std::endl;
                std::cout << "2) Marriage points (non-trump/trump) [" << nonTrumpMarriage << "/" << trumpMarriage << "]" << std::endl;
                std::cout << "3) Show players' points [" << (arePointsVisible ? "on" : "off") << "]" << std::endl;
                std::cout << "4) Last trick +10 [" << (lastTrickBonus ? "on" : "off") << "]" << std::endl;
                std::cout << "Enter number to change or 'back' to apply and return: ";

                char comm[MAX_STR_LEN];
                std::cin.getline(comm, MAX_STR_LEN);

                // Transforming comm toLower
                for (size_t i = 0; comm[i] != '\0'; ++i)
                {
                    comm[i] = std::tolower(comm[i]);
                }

                if (strcmp(comm, "1") == 0)
                {
                    while (true)
                    {
                        std::cout << "Enter a points target for the game: ";
                        char targetUpdateLine[MAX_STR_LEN];
                        std::cin.getline(targetUpdateLine, MAX_STR_LEN);
                        if (isDigit(targetUpdateLine))
                        {
                            requiredPointsToWin = std::atoi(targetUpdateLine);
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
                else if (strcmp(comm, "2") == 0)
                {
                    char line[MAX_STR_LEN];

                    while (true) // Non-trump marriage
                    {
                        std::cout << "Enter points for the non-trump marriage: ";
                        std::cin.getline(line, MAX_STR_LEN);

                        if (isDigit(line))
                        {
                            nonTrumpMarriage = std::atoi(line);
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
                        std::cin.getline(line, MAX_STR_LEN);

                        if (isDigit(line))
                        {
                            trumpMarriage = std::atoi(line);
                            break;
                        }
                        else
                        {
                            std::cout << "Invalid input. Try again" << std::endl;
                        }
                    }

                    std::cout << "Marriage points now (non-trump/trump) [" << nonTrumpMarriage << "/" << trumpMarriage << "]" << std::endl;
                    wereSettingsModified = true;
                }
                else if (strcmp(comm, "3") == 0)
                {
                    if (arePointsVisible)
                    {
                        const char *prompt = "Do you want to stop seeing players' points?";
                        if (getConfirmation(prompt))
                        {
                            arePointsVisible = false;
                        }
                    }
                    else
                    {
                        const char *prompt = "Do you want to start seeing players' points?";
                        if (getConfirmation(prompt))
                        {
                            arePointsVisible = true;
                        }
                    }
                    wereSettingsModified = true;
                }
                else if (strcmp(comm, "4") == 0)
                {
                    if (lastTrickBonus)
                    {
                        const char *prompt = "Do you want to stop the 10 bonus points for the last trick?";
                        if (getConfirmation(prompt))
                        {
                            lastTrickBonus = false;
                        }
                    }
                    else
                    {
                        const char *prompt = "Do you want to have 10 bonus points for the last trick?";
                        if (getConfirmation(prompt))
                        {
                            lastTrickBonus = true;
                        }
                    }
                    wereSettingsModified = true;
                }
                else if (strcmp(comm, "back") == 0)
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
                else
                {
                    std::cout << "Invalid command. Please try again." << std::endl;
                }
            }
        }
        else if (strcmp(firstCommWord, "hand") == 0)
        {
            std::cout << "Your hand (P" << currentPlayerId << "): ";

            if (currentPlayerId == 1)
            {
                printPlayerHand(P1Hand, P1HandSize);
                std::cout << std::endl;
            }
            else
            {
                printPlayerHand(P2Hand, P2HandSize);
                std::cout << std::endl;
            }
        }
        else if (strcmp(firstCommWord, "play") == 0)
        {
            const char playNumChar = fullComm[spacePos + 1];
            int index = playNumChar - '0';

            if (!isdigit(playNumChar) || index > 5 || index < 0)
            {
                std::cout << "Invalid card index" << std::endl;
                continue;
            }

            if (currentPlayerId == 1)
            {
                thrownCards[thrownCount++] = P1Hand[index];
                std::cout << "P1 played ";
                cardPrint(P1Hand[index]);
                std::cout << std::endl;
                // remove the card from P1's hand
                for (int k = index; k < P1HandSize - 1; ++k)
                    P1Hand[k] = P1Hand[k + 1];
                --P1HandSize;
            }
            else
            {
                thrownCards[thrownCount++] = P2Hand[index];
                std::cout << "P2 played ";
                cardPrint(P2Hand[index]);
                std::cout << std::endl;
                // remove the card from P2's hand
                for (int k = index; k < P2HandSize - 1; ++k)
                    P2Hand[k] = P2Hand[k + 1];
                --P2HandSize;
            }

            // If a player puts the first hand on the "table":
            if (thrownCount == 1)
            {
                firstPlayedPlayerId = currentPlayerId;
            }
            else if (thrownCount == 2) // If both players have played a card on the "table"
            {
                bool P1WinsTrick;
                Card firstCard = thrownCards[0];
                Card secondCard = thrownCards[1];

                if (isTrump(firstCard, trumpSuit) && !isTrump(secondCard, trumpSuit))
                {                                             // only first card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 1); // when first player plays first, he gives a trump
                }
                else if (!isTrump(secondCard, trumpSuit) && isTrump(firstCard, trumpSuit))
                {                                             // only second card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 1); // when second player played first, then first player played trump
                }
                else if (isTrump(secondCard, trumpSuit) && isTrump(firstCard, trumpSuit))
                {                                                      // Both cards are trump
                    P1WinsTrick = compareCards(firstCard, secondCard); // Stronger card wins
                }
                else
                { // Both cards are non-trump
                    // Changed to const char* and strcmp
                    const char *dominantSuit = getSuit(firstPlayedPlayerId == 1 ? firstCard : secondCard);
                    // Dominant suit is the first played card's suit

                    if (std::strcmp(getSuit(secondCard), dominantSuit) != 0)
                    { // If second card's suit doesn't match the first one
                        P1WinsTrick = (firstPlayedPlayerId == 1);
                        // If P1 played first, he wins the trick
                    }
                    else
                    {
                        P1WinsTrick = compareCards(firstCard, secondCard); // Stronger card wins
                    }
                }

                // CLOSED CHECK NEEDS TO BE ADDED!
                std::cout << (P1WinsTrick ? "P1" : "P2") << " wins the trick! ";

                thrownCount = 0;
            }

            currentPlayerId = 3 - currentPlayerId; // Looping the current player's turn
        }
        else if (strcmp(firstCommWord, "switch-nine") == 0)
        {
        }
        else if (strcmp(firstCommWord, "marriage") == 0)
        {
        }
        else if (strcmp(firstCommWord, "close") == 0)
        {
        }
        else if (strcmp(firstCommWord, "last-trick") == 0)
        {
        }
        else if (strcmp(firstCommWord, "trump") == 0)
        {
        }
        else if (strcmp(firstCommWord, "history") == 0)
        {
        }
        else if (strcmp(firstCommWord, "status") == 0)
        {
        }
        else if (strcmp(firstCommWord, "stop") == 0)
        {
        }
        else if (strcmp(firstCommWord, "surrender") == 0)
        {
        }
        else if (strcmp(firstCommWord, "surrender-forever") == 0)
        {
        }
        else if (strcmp(firstCommWord, "save") == 0)
        {
        }
        else if (strcmp(firstCommWord, "load") == 0)
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
