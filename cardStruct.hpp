#pragma once

constexpr int SUIT_MAX_LENGTH = 4; // UTF-8 (3 bytes) + "\0"
constexpr int RANK_MAX_LENGTH = 3; //"10" + "\0" at most

struct Card
{
    char suit[SUIT_MAX_LENGTH];
    char rank[RANK_MAX_LENGTH];

    int suitValue;
    int rankValue;
};