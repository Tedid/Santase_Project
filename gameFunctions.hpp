#pragma once
#include <vector>
// #include <fstream>
#include <algorithm>
#include <random>
#include <string>

#include "cardStruct.hpp"


bool isTrump(const std::string &card, const std::string &trumpSuit);
void initializeDeck(std::vector<std::string> &deck);
void dealCards(std::vector<std::string> &deck, std::vector<std::string> &P1, std::vector<std::string> &P2);
void revealTrump(std::vector<std::string> &deck, std::string &trumpSuit);
int getSuitValue(const std::string &card);
int getRankValue(const std::string &card);
bool compareCards(const std::string &card1, const std::string &card2);

std::string generateRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage);
std::string getSuit(std::string card);
std::string playerHand(const std::vector<std::string> &hand);

void cardPrint(const Card &card);

