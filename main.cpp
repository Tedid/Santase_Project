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

constexpr int DEFAULT_REQUIRED_POINTS_TO_WIN = 11;
constexpr int DEFAULT_NON_TRUMP_MARRIAGE = 20;
constexpr int DEFAULT_TRUMP_MARRIAGE = 40;
constexpr bool DEFAULT_ARE_POINTS_VISIBLE = true;
constexpr bool DEFAULT_LAST_TRICK_BONUS = true;

constexpr int THROWN_CARDS_MAX_NUMBER = 2;
constexpr int NUMBER_OF_RANKS = 6;

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

    // A=11, 10=10, K=4, Q=3, J=2, 9=0 points
    const int RANK_POINTS[] = {11, 10, 4, 3, 2, 0};

    int currentPlayerId = 1; // 1 for P1, 2 for P2
    int lastRoundWonPlayerId = 1;
    bool isStockClosed = false;
    char declaredMarriageSuit[SUIT_MAX_LENGTH];         // Stores the suit of the last declared marriage
    bool isMarriageDeclaredAndCardMustBePlayed = false; // True if a marriage was declared and one of its cards must be played
    bool manualStopCall = false;

    Card lastTrickCards[THROWN_CARDS_MAX_NUMBER];
    int lastTrickWinnerId = 0;

    GameHistory history;

    // REMINDER FOR ME TO ADD COLOURING TO TRUMP CARDS

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
            std::cout << "P" << currentPlayerId << "'s turn:";

            if (arePointsVisible)
            {
                std::cout << "\t\t(" << (currentPlayerId == 1 ? P1RoundPoints : P2RoundPoints) << " points)";
            }

            std::cout << std::endl;
            if (currentPlayerId == 1)
            {
                if (P1HandSize > 0)
                {
                    std::cout << "       "; // 7x spaces
                    for (int i = 0; i < P1HandSize; i++)
                    {
                        std::cout << "[" << i << "] ";
                    }
                    std::cout << std::endl;
                }
                std::cout << "Hand: ";
                printPlayerHand(P1Hand, P1HandSize);
                std::cout << std::endl;
            }

            else
            {
                if (P2HandSize > 0)
                {
                    std::cout << "       "; // 7x spaces
                    for (int i = 0; i < P2HandSize; i++)
                    {
                        std::cout << "[" << i << "] ";
                    }
                    std::cout << std::endl;
                }

                std::cout << "Hand: ";
                printPlayerHand(P2Hand, P2HandSize);
                std::cout << std::endl;
            }

            std::cout << "Trump suit: ";
            printSuitColored(trumpSuit);
            std::cout << std::endl;
            if (deckSize > 0)
            {
                std::cout << "Bottom card: ";
                cardPrint(deck[0]);
                std::cout << std::endl;
            }
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
                const char *PROMPT = "This will reset this whole game and start it from scratch. Continue?";
                if (!getConfirmation(PROMPT))
                {
                    continue; // Dismiss the command
                }
            }

            if (wereSettingsModified)
            {
                const char *PROMPT = "Do you want to reset the settings to their defaults?";
                if (getConfirmation(PROMPT))
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
            P1hasWonCard = false;
            P2hasWonCard = false;
            isStockClosed = false;
            isMarriageDeclaredAndCardMustBePlayed = false;
            lastTrickWinnerId = 0;
            initializeDeck(deck, deckSize); // Shuffling deck
            distributeCards(deck, P1Hand, P2Hand, deckSize);
            P1HandSize = HAND_MAX_SIZE;
            P2HandSize = HAND_MAX_SIZE;
            revealTrump(deck, deckSize, trumpSuit); // Top card goes under and becomes a trump
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
                        const char *PROMPT = "Do you want to stop seeing players' points?";
                        if (getConfirmation(PROMPT))
                        {
                            arePointsVisible = false;
                        }
                    }
                    else
                    {
                        const char *PROMPT = "Do you want to start seeing players' points?";
                        if (getConfirmation(PROMPT))
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
                        const char *PROMPT = "Do you want to stop the 10 bonus points for the last trick?";
                        if (getConfirmation(PROMPT))
                        {
                            lastTrickBonus = false;
                        }
                    }
                    else
                    {
                        const char *PROMPT = "Do you want to have 10 bonus points for the last trick?";
                        if (getConfirmation(PROMPT))
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
            const char PLAY_NUM_CHAR = fullComm[spacePos + 1];
            int index = PLAY_NUM_CHAR - '0';

            int currentPlayerHandSize = (currentPlayerId == 1 ? P1HandSize : P2HandSize);
            if (!isdigit(PLAY_NUM_CHAR) || index >= currentPlayerHandSize || index < 0)
            {
                std::cout << "Invalid card index" << std::endl;
                continue;
            }

            // Marriage obligation check
            if (isMarriageDeclaredAndCardMustBePlayed)
            {
                Card cardToPlay = (currentPlayerId == 1 ? P1Hand[index] : P2Hand[index]);
                if (strcmp(cardToPlay.suit, declaredMarriageSuit) != 0 ||
                    (strcmp(cardToPlay.rank, "K") != 0 && strcmp(cardToPlay.rank, "Q") != 0))
                {
                    std::cout << "You must play either the King or Queen of the declared marriage suit (";
                    printSuitColored(declaredMarriageSuit);
                    std::cout << ")!" << std::endl;
                    continue; // Prevent playing the wrong card
                }
                // If the correct card is played, reset the flag
                isMarriageDeclaredAndCardMustBePlayed = false;
            }

            bool playSuccessful = false;
            if (currentPlayerId == 1)
            {
                playSuccessful = processPlayerCardPlay(P1Hand, P1HandSize, thrownCards, thrownCount, trumpSuit, index, 1, isStockClosed);
            }
            else // currentPlayerId == 2
            {
                playSuccessful = processPlayerCardPlay(P2Hand, P2HandSize, thrownCards, thrownCount, trumpSuit, index, 2, isStockClosed);
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
                    const char *DOMINANT_SUIT = getSuit(firstPlayedPlayerId == 1 ? firstCard : secondCard);
                    // Dominant suit is the first played card's suit

                    if (std::strcmp(getSuit(secondCard), DOMINANT_SUIT) != 0)
                    { // If second card's suit doesn't match the first one
                        P1WinsTrick = (firstPlayedPlayerId == 1);
                        // If P1 played first, he wins the trick
                    }
                    else
                    {
                        P1WinsTrick = compareCards(firstCard, secondCard); // Stronger card wins
                    }
                }

                int wonPoints = RANK_POINTS[NUMBER_OF_RANKS - firstCard.rankValue] + RANK_POINTS[NUMBER_OF_RANKS - secondCard.rankValue];
                if (P1WinsTrick)
                {
                    P1hasWonCard = true;
                    P1RoundPoints += wonPoints;
                }
                else
                {
                    P2hasWonCard = true;
                    P2RoundPoints += wonPoints;
                }

                std::cout << (P1WinsTrick ? "P1" : "P2") << " wins the trick! ";

                if (wonPoints > 0)
                {
                    std::cout << "(+" << wonPoints << " points)" << std::endl;
                }

                currentPlayerId = (P1WinsTrick ? 1 : 2); // Winner plays next

                // Store last trick information
                lastTrickCards[0] = thrownCards[0];
                lastTrickCards[1] = thrownCards[1];
                lastTrickWinnerId = currentPlayerId;

                // Deal cards if the round is not closed
                if (!isStockClosed)
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

                    // Check if the deck is now empty and close the stock
                    if (deckSize == 0)
                    {
                        isStockClosed = true;
                        std::cout << "Deck is empty. Stock closed. Strict rules are now in effect." << std::endl;
                    }
                }

                thrownCount = 0;
            }

            // ROUND END
            if (P1HandSize == 0 && P2HandSize == 0)
            {
                manualStopCall = false;
                int roundWinnerId = roundEnd(manualStopCall, lastTrickWinnerId, P1RoundPoints, P2RoundPoints, P1hasWonCard, P2hasWonCard, P1GamePoints, P2Gamepoints);

                // Check for game winner
                if (P1GamePoints >= requiredPointsToWin || P2Gamepoints >= requiredPointsToWin)
                {
                    gameOver = true;
                    std::cout << "Game Over! Player " << (P1GamePoints >= requiredPointsToWin ? "1" : "2") << " wins the match!" << std::endl;
                    break;
                }

                P1RoundPoints = 0;
                P2RoundPoints = 0;
                P1hasWonCard = false;
                P2hasWonCard = false;
                isStockClosed = false;
                isMarriageDeclaredAndCardMustBePlayed = false;
                lastTrickWinnerId = 0;

                initializeDeck(deck, deckSize); // Shuffle for next round
                distributeCards(deck, P1Hand, P2Hand, deckSize);
                P1HandSize = HAND_MAX_SIZE;
                P2HandSize = HAND_MAX_SIZE;
                revealTrump(deck, deckSize, trumpSuit);
                currentPlayerId = roundWinnerId; // Winner of previous round starts the new one
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

            if (isStockClosed)
            {
                std::cout << "Stock is closed, you can't switch-nine." << std::endl;
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

                    std::cout << "You exchanged ";
                    cardPrint(nine_Trump);
                    std::cout << " for ";
                    cardPrint(currentCard);
                    std::cout << " (trump suit)" << std::endl;

                    std::sort((currentPlayerId == 1 ? P1Hand : P2Hand), (currentPlayerId == 1 ? P1Hand + P1HandSize : P2Hand + P2HandSize), compareCards);

                    break;
                }
            }
            if (!foundNineOfTrump)
            {
                std::cout << "Player " << (currentPlayerId == 1 ? "1" : "2") << " doesn't have the ";
                cardPrint(nine_Trump);
                std::cout << std::endl;
            }
        }
        else if (strcmp(firstCommWord, "marriage") == 0)
        {
            if (isStockClosed)
            {
                std::cout << "Stock is closed, you cannot declare a marriage." << std::endl;
                continue;
            }
            if (thrownCount == 1)
            { // If a card has already been played in this trick
                std::cout << "You cannot declare a marriage after the first card of the trick has been played." << std::endl;
                continue;
            }
            if (isMarriageDeclaredAndCardMustBePlayed)
            {
                std::cout << "You have already declared a marriage and must play one of its cards." << std::endl;
                continue;
            }

            // Check if the current player has won at least one trick
            if (!(currentPlayerId == 1 ? P1hasWonCard : P2hasWonCard))
            {
                std::cout << "You must have won at least one trick to declare a marriage." << std::endl;
                continue;
            }

            // Check for the very first trick of the round (no player has won cards)
            if (!P1hasWonCard && !P2hasWonCard)
            {
                std::cout << "You cannot declare a marriage during the first trick of the round." << std::endl;
                continue;
            }

            char suitChar = fullComm[spacePos + 1];
            char targetSuit[SUIT_MAX_LENGTH];
            bool isValidSuitChar = true;

            switch (std::toupper(suitChar))
            {
            case 'H':
                std::strcpy(targetSuit, "♥");
                break;
            case 'D':
                std::strcpy(targetSuit, "♦");
                break;
            case 'C':
                std::strcpy(targetSuit, "♣");
                break;
            case 'S':
                std::strcpy(targetSuit, "♠");
                break;
            default:
                isValidSuitChar = false;
                break;
            }

            if (!isValidSuitChar)
            {
                std::cout << "Invalid suit specified. Use H(";
                printSuitColored("♥");
                std::cout << "), D(";
                printSuitColored("♦");
                std::cout << "), C(";
                printSuitColored("♣");
                std::cout << "), or S(";
                printSuitColored("♠");
                std::cout << ")." << std::endl;
                continue;
            }

            Card *currentHand = (currentPlayerId == 1 ? P1Hand : P2Hand);
            int currentHandSize = (currentPlayerId == 1 ? P1HandSize : P2HandSize);

            bool hasKing = false;
            bool hasQueen = false;

            for (int i = 0; i < currentHandSize; ++i)
            {
                if (strcmp(currentHand[i].suit, targetSuit) == 0)
                {
                    if (strcmp(currentHand[i].rank, "K") == 0)
                    {
                        hasKing = true;
                    }
                    else if (strcmp(currentHand[i].rank, "Q") == 0)
                    {
                        hasQueen = true;
                    }
                }
            }

            if (hasKing && hasQueen)
            {
                int pointsEarned = 0;
                if (strcmp(targetSuit, trumpSuit) == 0)
                {
                    pointsEarned = trumpMarriage;
                    std::cout << "Marriage declared: K";
                    printSuitColored(targetSuit);
                    std::cout << " + Q";
                    printSuitColored(targetSuit);
                    std::cout << " (trump suit)" << std::endl;
                }
                else
                {
                    pointsEarned = nonTrumpMarriage;
                    std::cout << "Marriage declared: K";
                    printSuitColored(targetSuit);
                    std::cout << " + Q";
                    printSuitColored(targetSuit);
                    std::cout << std::endl;
                }

                if (currentPlayerId == 1)
                {
                    P1RoundPoints += pointsEarned;
                }
                else
                {
                    P2RoundPoints += pointsEarned;
                }

                std::cout << "You earned " << pointsEarned << " points." << std::endl;
                std::cout << "You must play K";
                printSuitColored(targetSuit);
                std::cout << " or Q";
                printSuitColored(targetSuit);
                std::cout << std::endl;

                isMarriageDeclaredAndCardMustBePlayed = true;
                std::strcpy(declaredMarriageSuit, targetSuit);
            }
            else
            {
                std::cout << "You don't have both the King and Queen of ";
                printSuitColored(targetSuit);
                std::cout << " in your hand." << std::endl;
            }
        }
        else if (strcmp(firstCommWord, "close") == 0)
        {
            isStockClosed = true;
            std::cout << "Stock closed. No more cards will be drawn.\nStrict rules are now in effect" << std::endl;
        }
        else if (strcmp(firstCommWord, "last-trick") == 0)
        {
            if (lastTrickWinnerId == 0)
            {
                std::cout << "No tricks have been played yet." << std::endl;
            }
            else
            {
                std::cout << "Player " << firstPlayedPlayerId << ": ";
                cardPrint(lastTrickCards[0]);
                std::cout << std::endl;
                std::cout << "Player " << (3 - firstPlayedPlayerId) << ": ";
                cardPrint(lastTrickCards[1]);
                std::cout << std::endl;
                std::cout << "Winner: Player " << lastTrickWinnerId << std::endl;
            }
        }
        else if (strcmp(firstCommWord, "trump") == 0)
        {
            std::cout << "Trump suit: ";
            printSuitColored(trumpSuit);
            std::cout << std::endl;
        }
        else if (strcmp(firstCommWord, "history") == 0)
        {
            printHistory(history);
        }
        else if (strcmp(firstCommWord, "status") == 0)
        {
            if (!hasGameStarted)
            {
                std::cout << "The game has not started yet. Type 'start' to begin." << std::endl;
                continue;
            }
            std::cout << "--- CURRENT GAME STATUS ---" << std::endl;
            std::cout << "Player 1 Game Points: " << P1GamePoints << std::endl;
            std::cout << "Player 2 Game Points: " << P2Gamepoints << std::endl;
            std::cout << "Player 1 Round Points: " << P1RoundPoints << std::endl;
            std::cout << "Player 2 Round Points: " << P2RoundPoints << std::endl;
            std::cout << "Current Player: P" << currentPlayerId << std::endl;
            std::cout << "Trump Suit: ";
            printSuitColored(trumpSuit);
            std::cout << std::endl;
            std::cout << "Cards left in deck: " << deckSize << std::endl;
            if (deckSize > 0)
            {
                std::cout << "Bottom card of deck: ";
                cardPrint(deck[0]);
                std::cout << std::endl;
            }
            else
            {
                std::cout << "Deck is empty." << std::endl;
            }
            std::cout << "Stock is " << (isStockClosed ? "CLOSED" : "OPEN") << std::endl;
            if (isMarriageDeclaredAndCardMustBePlayed)
            {
                std::cout << "Marriage declared for ";
                printSuitColored(declaredMarriageSuit);
                std::cout << ". P" << currentPlayerId << " must play a K or Q of this suit." << std::endl;
            }
        }
        else if (strcmp(firstCommWord, "stop") == 0)
        {
            manualStopCall = true;
            int roundWinnerId = roundEnd(manualStopCall, lastTrickWinnerId, P1RoundPoints, P2RoundPoints, P1hasWonCard, P2hasWonCard, P1GamePoints, P2Gamepoints);

            // Check for game winner
            if (P1GamePoints >= requiredPointsToWin || P2Gamepoints >= requiredPointsToWin)
            {
                gameOver = true;
                std::cout << "Game Over! Player " << (P1GamePoints >= requiredPointsToWin ? "1" : "2") << " wins the match!" << std::endl;
                break;
            }

            P1RoundPoints = 0;
            P2RoundPoints = 0;
            P1hasWonCard = false;
            P2hasWonCard = false;
            isStockClosed = false;
            isMarriageDeclaredAndCardMustBePlayed = false;
            lastTrickWinnerId = 0;

            initializeDeck(deck, deckSize);
            distributeCards(deck, P1Hand, P2Hand, deckSize);
            P1HandSize = HAND_MAX_SIZE;
            P2HandSize = HAND_MAX_SIZE;
            revealTrump(deck, deckSize, trumpSuit);
            currentPlayerId = roundWinnerId; // Winner of the previous round starts the new one
        }
        else if (strcmp(firstCommWord, "surrender") == 0)
        {
            const char *PROMPT = "Are you sure you want to surrender for the round?";
            if (getConfirmation(PROMPT))
            {

                (currentPlayerId == 1 ? P2RoundPoints : P1RoundPoints) = 666; // A very big number to ensure opponent wins

                manualStopCall = true;
                int roundWinnerId = roundEnd(manualStopCall, lastTrickWinnerId, P1RoundPoints, P2RoundPoints, P1hasWonCard, P2hasWonCard, P1GamePoints, P2Gamepoints);

                // Check for game winner
                if (P1GamePoints >= requiredPointsToWin || P2Gamepoints >= requiredPointsToWin)
                {
                    gameOver = true;
                    std::cout << "Game Over! Player " << (P1GamePoints >= requiredPointsToWin ? "1" : "2") << " wins the match!" << std::endl;
                    break;
                }

                P1RoundPoints = 0;
                P2RoundPoints = 0;
                P1hasWonCard = false;
                P2hasWonCard = false;
                isStockClosed = false;
                isMarriageDeclaredAndCardMustBePlayed = false;
                lastTrickWinnerId = 0;

                initializeDeck(deck, deckSize);
                distributeCards(deck, P1Hand, P2Hand, deckSize);
                P1HandSize = HAND_MAX_SIZE;
                P2HandSize = HAND_MAX_SIZE;
                revealTrump(deck, deckSize, trumpSuit);
                currentPlayerId = roundWinnerId; // Winner of the previous round starts the new one
            }
        }
        else if (strcmp(firstCommWord, "surrender-forever") == 0)
        {
            const char *PROMPT = "Are you sure you want to surrender for the whole game?";
            if (getConfirmation(PROMPT))
            {
                gameOver = true;
                std::cout << "Game Over! Player " << (currentPlayerId == 1 ? "2" : "1") << " wins the match!" << std::endl;
                break;
            }
        }
        else if (strcmp(firstCommWord, "save") == 0)
        {
        }
        else if (strcmp(firstCommWord, "load") == 0)
        {
        }
        else
        {
            std::cout << "Unknown command. Please try again." << std::endl;
            continue;
        }

        std::cout << std::endl;
    }
}
