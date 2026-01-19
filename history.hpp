#pragma once

struct RoundHistory
{
    int roundNumber = 1;
    int winnerId;
    int gamePointsWon;
    int p1Score;
    int p2Score;
    bool isOngoing = true;
};

constexpr int MAX_ROUNDS = 100;

struct GameHistory
{
    RoundHistory history[MAX_ROUNDS];
    int totalRounds = 0;
    int overallP1 = 0;
    int overallP2 = 0;
    // int currentPlayerId = 1;
};
