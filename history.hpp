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
 * Header file containing struct declarations used in the main logic.
 *
 */


#pragma once

struct RoundHistory {
  int roundNumber = 1;
  int winnerId;
  int gamePointsWon;
  int p1Score;
  int p2Score;
  bool isOngoing = true;
};

constexpr int MAX_ROUNDS = 100;

struct GameHistory {
  RoundHistory history[MAX_ROUNDS];
  int totalRounds = 0;
  int overallP1 = 0;
  int overallP2 = 0;
  // int currentPlayerId = 1;
};
