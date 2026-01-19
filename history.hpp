#pragma once

struct RoundHistory
{
    int roundNumber;
    int winnerId;
    int gamePointsWon;
    int p1Score;
    int p2Score;
    bool isOngoing;
};

constexpr int MAX_ROUNDS = 100;

struct GameHistory
{
    RoundHistory history[MAX_ROUNDS];
    int totalRounds;
    int overallP1;
    int overallP2;
    int currentPlayerId;
};
