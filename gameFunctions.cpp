#include "gameFunctions.hpp"

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

bool isTrump(const std::string &card, const std::string &trumpSuit)
{
    if (trumpSuit.empty())
        return false;
    return card.back() == trumpSuit.back();
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
