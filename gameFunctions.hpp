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
 * Header file containing function declarations for the main logic.
 *
 */


#pragma once
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>

#include "cardStruct.hpp"
#include "history.hpp"

constexpr int DECK_MAX_SIZE = 24;
constexpr int HAND_MAX_SIZE = 6;

constexpr int THROWN_CARDS_MAX_NUMBER = 2;

bool isTrump(const Card& card, const char* trumpSuit);
void initializeDeck(Card deck[DECK_MAX_SIZE], int& deckSize);
void distributeCards(Card deck[DECK_MAX_SIZE], Card P1Hand[], Card P2Hand[], int& deckSize);
void deckShuffle(Card deck[DECK_MAX_SIZE], int deckSize);
void deckSort(Card deck[DECK_MAX_SIZE], int deckSize);
void revealTrump(Card deck[DECK_MAX_SIZE], int& deckSize, char* trumpSuit);
int getSuitValue(const Card& card);
int getRankValue(const Card& card);
bool compareCards(const Card& card1, const Card& card2);

void printRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage);
const char* getSuit(const Card& card);
int dealCard(Card deck[DECK_MAX_SIZE], Card PHand[], int& deckSize, int& handSize);
void printSuitColored(const char* suit);
void printYellowWordTrump();
void printCardIndexes(Card hand[HAND_MAX_SIZE], int handSize, const char* trumpSuit);
void cardPrint(const Card& card);
void printPlayerHand(const Card hand[HAND_MAX_SIZE], size_t size);
void printHistory(const GameHistory& history);

bool isValidPlayWhenClosed(const Card* playerHand, int playerHandSize, const Card& opponentCard, const char* trumpSuit,
                           int playedIndex);
bool processPlayerCardPlay(Card playerHand[], int& playerHandSize, Card thrownCards[], int& thrownCount, const char trumpSuit[],
                           int cardIndex, int playerId, bool isRoundClosed);
int roundEnd(bool manualStopCall, int lastTrickWinnerId, int& P1RoundPoints, int& P2RoundPoints, bool P1hasWonCard,
             bool P2hasWonCard, int& P1GamePoints, int& P2GamePoints, bool lastTrickBonus);
void startNewRoundHistory(GameHistory& game, int currentRoundNumber);
void finalizeCurrentRoundHistory(GameHistory& game, int winnerId, int gamePointsAwardedThisRound, int p1RoundScore,
                                 int p2RoundScore);

bool saveGameState(const char* filename, bool hasGameStarted, bool wereSettingsModified, const char trumpSuit[SUIT_MAX_LENGTH],
                   const Card deck[DECK_MAX_SIZE], int deckSize, int currentRoundNumber, const Card P1Hand[HAND_MAX_SIZE],
                   int P1HandSize, const Card P2Hand[HAND_MAX_SIZE], int P2HandSize,
                   const Card thrownCards[THROWN_CARDS_MAX_NUMBER], int thrownCount, int firstPlayedPlayerId, int P1GamePoints,
                   int P2GamePoints, bool P1hasWonCard, bool P2hasWonCard, int P1RoundPoints, int P2RoundPoints,
                   int currentPlayerId, int lastRoundWonPlayerId, bool isStockClosed,
                   const char declaredMarriageSuit[SUIT_MAX_LENGTH], bool isMarriageDeclaredAndCardMustBePlayed,
                   bool manualStopCall, const Card lastTrickCards[THROWN_CARDS_MAX_NUMBER], int lastTrickWinnerId,
                   const GameHistory& history, int requiredPointsToWin, int nonTrumpMarriage, int trumpMarriage,
                   bool arePointsVisible, bool lastTrickBonus);

bool loadGameState(const char* filename, bool& hasGameStarted, bool& wereSettingsModified, char trumpSuit[SUIT_MAX_LENGTH],
                   Card deck[DECK_MAX_SIZE], int& deckSize, int& currentRoundNumber, Card P1Hand[HAND_MAX_SIZE], int& P1HandSize,
                   Card P2Hand[HAND_MAX_SIZE], int& P2HandSize, Card thrownCards[THROWN_CARDS_MAX_NUMBER], int& thrownCount,
                   int& firstPlayedPlayerId, int& P1GamePoints, int& P2GamePoints, bool& P1hasWonCard, bool& P2hasWonCard,
                   int& P1RoundPoints, int& P2RoundPoints, int& currentPlayerId, int& lastRoundWonPlayerId, bool& isStockClosed,
                   char declaredMarriageSuit[SUIT_MAX_LENGTH], bool& isMarriageDeclaredAndCardMustBePlayed, bool& manualStopCall,
                   Card lastTrickCards[THROWN_CARDS_MAX_NUMBER], int& lastTrickWinnerId, GameHistory& history,
                   int& requiredPointsToWin, int& nonTrumpMarriage, int& trumpMarriage, bool& arePointsVisible,
                   bool& lastTrickBonus);
