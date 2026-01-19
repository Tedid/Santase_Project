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
 * Header file containing a struct declaration used in the main logic.
 *
 */

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