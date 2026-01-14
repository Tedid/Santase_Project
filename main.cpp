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

// constexpr int DECK_MAX_SIZE = 24;
// constexpr int HAND_MAX_SIZE = 6;
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
    bool P1hasWonCard = false, P2hasWonCard = false;
    int P1RoundPoints = 0, P2RoundPoints = 0;

    int currentPlayerId = 1; // 1 for P1, 2 for P2
    int lastRoundWonPlayerId = 1;
    bool isRoundClosed = false;

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
            distributeCards(deck, P1Hand, P2Hand, deckSize);
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

            int currentPlayerHandSize = (currentPlayerId == 1 ? P1HandSize : P2HandSize);
            if (!isdigit(playNumChar) || index >= currentPlayerHandSize || index < 0)
            {
                std::cout << "Invalid card index" << std::endl;
                continue;
            }

            bool playSuccessful = false;
            if (currentPlayerId == 1)
            {
                playSuccessful = processPlayerCardPlay(P1Hand, P1HandSize, thrownCards, thrownCount, trumpSuit, index, 1, isRoundClosed);
            }
            else // currentPlayerId == 2
            {
                playSuccessful = processPlayerCardPlay(P2Hand, P2HandSize, thrownCards, thrownCount, trumpSuit, index, 2, isRoundClosed);
            }

            if (!playSuccessful)
            {
                continue; // Skip the rest of the play logic if the card wasn't played successfully
            }

            // If a player puts the first hand on the "table":
            if (thrownCount == 1)
            {
                firstPlayedPlayerId = currentPlayerId;
                currentPlayerId = 3 - currentPlayerId; // Switching the current player's turn
            }
            else if (thrownCount == 2) // If both players have played a card on the "table"
            {
                bool P1WinsTrick;
                Card firstCard = thrownCards[0];
                Card secondCard = thrownCards[1];

                if (isTrump(firstCard, trumpSuit) && !isTrump(secondCard, trumpSuit))
                {                                             // only first card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 1); // when first player plays first, he gives the trump
                }
                else if (!isTrump(firstCard, trumpSuit) && isTrump(secondCard, trumpSuit))
                {                                             // only second card is a trump
                    P1WinsTrick = (firstPlayedPlayerId == 2); // when second player played first, then first player played trump
                }
                else if (isTrump(secondCard, trumpSuit) && isTrump(firstCard, trumpSuit))
                {                                                      // Both cards are trump
                    P1WinsTrick = compareCards(firstCard, secondCard); // Stronger card wins
                }
                else
                { // Both cards are non-trump
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

                if (P1WinsTrick)
                {
                    P1hasWonCard = true;
                }
                else
                {
                    P2hasWonCard = true;
                }

                std::cout << (P1WinsTrick ? "P1" : "P2") << " wins the trick! ";
                currentPlayerId = (P1WinsTrick ? 1 : 2); // Winner plays next

                // Deal cards if the round is not closed
                if (!isRoundClosed)
                {
                    if (deckSize > 0)
                    {
                        // Winner draws the first card
                        if (currentPlayerId == 1)
                        {
                            dealCard(deck, P1Hand, deckSize, P1HandSize);
                        }
                        else
                        {
                            dealCard(deck, P2Hand, deckSize, P2HandSize);
                        }
                    }
                    if (deckSize > 0)
                    {
                        // The other player draws the second card
                        if (currentPlayerId == 1)
                        { // P1 won, P2 gets the next card
                            dealCard(deck, P2Hand, deckSize, P2HandSize);
                        }
                        else
                        { // P2 won, P1 gets the next card
                            dealCard(deck, P1Hand, deckSize, P1HandSize);
                        }
                    }
                }

                thrownCount = 0;
            }
        }
        else if (strcmp(firstCommWord, "switch-nine") == 0)
        {
            // If player hasn't yet won a card, they can't swap cards :)
            if (!(currentPlayerId == 1 ? P1hasWonCard : P2hasWonCard))
            {
                std::cout << "Player " << (currentPlayerId == 1 ? "1" : "2") << " doesn't have a won set of cards yet!" << std::endl;
                continue;
            }

            if (isRoundClosed)
            {
                std::cout << "Round is closed, you can't switch-nine." << std::endl;
                continue;
            }

            if (deckSize == 0)
            {
                std::cout << "Deck is empty, you can't switch-nine" << std::endl;
                continue;
            }

            if (deckSize >= 2)
            {
                std::cout << "Deck has only two cards, you can't switch-nine" << std::endl;
                continue;
            }

            Card nine_Trump;
            std::strncpy(nine_Trump.suit, trumpSuit, SUIT_MAX_LENGTH);
            nine_Trump.suit[SUIT_MAX_LENGTH - 1] = '\0';
            std::strncpy(nine_Trump.rank, "9", RANK_MAX_LENGTH);
            nine_Trump.rank[RANK_MAX_LENGTH - 1] = '\0';
            // compute values
            nine_Trump.suitValue = getSuitValue(nine_Trump);
            nine_Trump.rankValue = getRankValue(nine_Trump);

            bool foundNineOfTrump = false;

            for (int i = 0; i < (currentPlayerId == 1 ? P1HandSize : P2HandSize); i++)
            {
                Card &currentCard = (currentPlayerId == 1 ? P1Hand[i] : P2Hand[i]);
                if (strcmp(currentCard.suit, nine_Trump.suit) == 0 &&
                    strcmp(currentCard.rank, nine_Trump.rank) == 0)
                {
                    foundNineOfTrump = true;

                    // Swapping the player's 9 of trump with the bottom deck card
                    Card tempCardInHand = currentCard;
                    currentCard = deck[0];
                    deck[0] = tempCardInHand;

                    std::cout << "Successfully switched 9 of trump with the bottom card." << std::endl;

                    std::sort((currentPlayerId == 1 ? P1Hand : P2Hand), (currentPlayerId == 1 ? P1Hand + P1HandSize : P2Hand + P2HandSize), compareCards);

                    break;
                }
            }
            if (!foundNineOfTrump)
            {
                std::cout << "Player " << (currentPlayerId == 1 ? "1" : "2") << " doesn't have the 9 of trump!" << std::endl;
            }
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
