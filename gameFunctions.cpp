#include "gameFunctions.hpp"

const char *COLOR_RED = "\033[31m";
const char *COLOR_BLACK = "\033[30m";
const char *COLOR_RESET = "\033[0m";

const char *HEARTS = "♥";
const char *DIAMONDS = "♦";
const char *CLUBS = "♣";
const char *SPADES = "♠";

// Sorting priorities:----------------------
const char *SUIT_CHARS = "♠♥♦♣";
const int SUIT_VALUES[] = {4, 3, 2, 1};
// ♠=4, ♥=3, ♦=2, ♣=1

const char *RANK_CHARS = "A1KQJ9"; // "1" represents "10"
const int RANK_VALUES[] = {6, 5, 4, 3, 2, 1};
// A=6, 10=5, K=4, Q=3, J=2, 9=1
//-----------------------------------------

#include <cstring>

bool isTrump(const Card &card, const char *trumpSuit)
{
    if (!trumpSuit || trumpSuit[0] == '\0')
        return false;

    return std::strcmp(card.suit, trumpSuit) == 0;
}

void initializeDeck(Card deck[DECK_SIZE], int &deckSize)
{

    const char *suits[] = {"♣", "♦", "♥", "♠"};
    const char *ranks[] = {"9", "J", "Q", "K", "10", "A"};

    int idx = 0;
    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 6; ++i)
        {
            std::strncpy(deck[idx].suit, suits[j], SUIT_MAX_LENGTH);
            deck[idx].suit[SUIT_MAX_LENGTH - 1] = '\0';
            std::strncpy(deck[idx].rank, ranks[i], RANK_MAX_LENGTH);
            deck[idx].rank[RANK_MAX_LENGTH - 1] = '\0';
            // compute values
            deck[idx].suitValue = getSuitValue(deck[idx]);
            deck[idx].rankValue = getRankValue(deck[idx]);
            ++idx;
        }
    }

    deckSize = DECK_SIZE;

    // SHUFFLING DECK
    std::shuffle(deck, deck + DECK_SIZE, std::mt19937{std::random_device{}()});

    // // Recompute values in case shuffle or platform affected bytes
    // for (size_t i = 0; i < DECK_SIZE; ++i)
    // {
    //     deck[i].suitValue = getSuitValue(deck[i]);
    //     deck[i].rankValue = getRankValue(deck[i]);
    // }
}

void dealCards(Card deck[DECK_SIZE], Card P1[], Card P2[], int &deckSize)
{
    deckSize = DECK_SIZE;
    int p1Idx = 0, p2Idx = 0;

    for (int j = 0; j < 2; ++j)
    {
        for (int i = 0; i < 3; ++i)
        {
            // take from the top (deck[DECK_SIZE-1])
            P1[p1Idx++] = deck[--deckSize];
        }
        for (int i = 0; i < 3; ++i)
        {
            P2[p2Idx++] = deck[--deckSize];
        }
    }

    // Sorting hands:
    std::sort(P1, P1 + HAND_SIZE, compareCards);
    std::sort(P2, P2 + HAND_SIZE, compareCards);
}

void revealTrump(Card deck[DECK_SIZE], char *trumpSuit)
{
    Card topCard = deck[DECK_SIZE - 1];
    // shift right by one
    for (size_t i = DECK_SIZE - 1; i >= 1; i--)
    {
        deck[i] = deck[i - 1];
        if (i == 1)
            break; // size_t safety for unsigned wrap
    }
    deck[0] = topCard;

    // set trump suit (raw suit symbol)
    std::strncpy(trumpSuit, deck[0].suit, SUIT_MAX_LENGTH);
    trumpSuit[SUIT_MAX_LENGTH - 1] = '\0';
}

int getSuitValue(const Card &card)
{
    const char *suit_to_find = card.suit;

    // Loop for 4 suits
    for (int i = 0; i < 4; ++i)
    {
        char currentSuitChar[SUIT_MAX_LENGTH];
        std::strncpy(currentSuitChar, &SUIT_CHARS[i * 3], 3);
        currentSuitChar[3] = '\0'; // Ensure null termination

        if (std::strcmp(currentSuitChar, suit_to_find) == 0)
        {
            return SUIT_VALUES[i];
        }
    }
    return -1;
}

int getRankValue(const Card &card)
{
    char rankCharToFind = card.rank[0];

    const char *foundPtr = std::strchr(RANK_CHARS, rankCharToFind);

    if (foundPtr != nullptr)
    {
        return RANK_VALUES[foundPtr - RANK_CHARS];
    }
    return -1; // Rank not found or invalid
}

bool compareCards(const Card &card1, const Card &card2)
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

void printRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage)
{

    std::cout << "------------ SANTASE GAME RULES ------------\n";
    std::cout << "* Each player gets 6 cards. The trump suit is the suit of the next card in the deck.\n";
    std::cout << "This card gets put below the deck.\n";
    std::cout << "* Card values: A=11, 10=10, K=4, Q=3, J=2, 9=0.\n";
    std::cout << "* A marriage (K+Q of the same suit) gives " + std::to_string(nonTrumpMarriage) + " points, or " + std::to_string(trumpMarriage) + " if of the trump suit.\n";
    std::cout << "* The winner of a trick leads next and draws a card from the stock.\n";
    std::cout << "* Players must follow suit and trump only after the stock has been closed.\n";
    std::cout << "* The first player to reach 66 points wins the round.\n";
    std::cout << "* The game finishes when a player hits " + std::to_string(requiredPointsToWin) + " match points.\n";
}

const char *getSuit(const Card &card)
{
    if (std::strcmp(card.suit, "♥") == 0)
        return HEARTS;
    else if (std::strcmp(card.suit, "♦") == 0)
        return DIAMONDS;
    else if (std::strcmp(card.suit, "♣") == 0)
        return CLUBS;
    else if (std::strcmp(card.suit, "♠") == 0)
        return SPADES;

    return card.suit;
}

void cardPrint(const Card &card)
{
    const char *colorCode = (card.suitValue == 3 || card.suitValue == 2) ? COLOR_RED : COLOR_BLACK;
    std::cout << colorCode << card.rank << getSuit(card) << COLOR_RESET;
}

void printPlayerHand(const Card hand[HAND_SIZE], size_t size)
{
    std::cout << "[ ";

    for (size_t i = 0; i < size; ++i)
    {
        std::cout << hand[i].rank << getSuit(hand[i]);

        if (i != size - 1)
            std::cout << ", ";
    }
    std::cout << " ]";
}
