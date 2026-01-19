#pragma once
// #include <fstream>
#include <iostream>
#include <random>
#include <cstring>

#include "cardStruct.hpp"
#include "history.hpp"

constexpr int DECK_MAX_SIZE = 24;
constexpr int HAND_MAX_SIZE = 6;

bool isTrump(const Card &card, const char *trumpSuit);
void initializeDeck(Card deck[DECK_MAX_SIZE], int &deckSize);
void distributeCards(Card deck[DECK_MAX_SIZE], Card P1Hand[], Card P2Hand[], int &deckSize);
void deckShuffle(Card deck[DECK_MAX_SIZE], int deckSize);
void deckSort(Card deck[DECK_MAX_SIZE], int deckSize);
void revealTrump(Card deck[DECK_MAX_SIZE], int &deckSize, char *trumpSuit);
int getSuitValue(const Card &card);
int getRankValue(const Card &card);
bool compareCards(const Card &card1, const Card &card2);

void printRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage);
const char *getSuit(const Card &card);
int dealCard(Card deck[DECK_MAX_SIZE], Card PHand[], int &deckSize, int &handSize);
void printSuitColored(const char *suit);
void cardPrint(const Card &card);
void printPlayerHand(const Card hand[HAND_MAX_SIZE], size_t size);
void printHistory(const GameHistory &history);

bool isValidPlayWhenClosed(const Card *playerHand, int playerHandSize, const Card &opponentCard, const char *trumpSuit, int playedIndex);
bool processPlayerCardPlay(Card playerHand[], int &playerHandSize, Card thrownCards[], int &thrownCount,
                           const char trumpSuit[], int cardIndex, int playerId, bool isRoundClosed);
int roundEnd(bool manualStopCall, int lastTrickWinnerId, int &P1RoundPoints, int &P2RoundPoints,
             bool P1hasWonCard, bool P2hasWonCard, int &P1GamePoints, int &P2GamePoints);
void startNewRoundHistory(GameHistory &game, int currentRoundNumber);
void finalizeCurrentRoundHistory(GameHistory &game, int winnerId, int gamePointsAwardedThisRound, int p1RoundScore, int p2RoundScore);