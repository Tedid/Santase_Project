#pragma once
// #include <fstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <cstddef> // For size_t
#include <sstream>

#include "cardStruct.hpp"


constexpr int DECK_SIZE = 24;
constexpr int HAND_SIZE = 6;

bool isTrump(const Card &card, const char *trumpSuit);
void initializeDeck(Card deck[DECK_SIZE], int &deckSize);
void distributeCards(Card deck[DECK_SIZE], Card P1Hand[], Card P2Hand[],int &deckSize);
void revealTrump(Card deck[DECK_SIZE], char *trumpSuit);
int getSuitValue(const Card &card);
int getRankValue(const Card &card);
bool compareCards(const Card &card1, const Card &card2);

void printRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage);
const char *getSuit(const Card &card);
void printSuitColored(const char *suit);
void cardPrint(const Card &card);
void printPlayerHand(const Card hand[HAND_SIZE], size_t size);

