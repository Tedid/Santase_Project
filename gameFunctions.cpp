#include "gameFunctions.hpp"

const char* COLOR_RED = "\033[31m";
const char* COLOR_BLACK = "\033[30m";
const char* COLOR_YELLOW = "\033[33m";
const char* COLOR_RESET = "\033[0m";

const char* HEARTS = "♥";
const char* DIAMONDS = "♦";
const char* CLUBS = "♣";
const char* SPADES = "♠";

// Sorting priorities:----------------------
const char* SUIT_CHARS = "♠♥♦♣";
const int SUIT_VALUES[] = {4, 3, 2, 1};
// ♠=4, ♥=3, ♦=2, ♣=1

const char* RANK_CHARS = "A1KQJ9";  // "1" represents "10"
const int RANK_VALUES[] = {6, 5, 4, 3, 2, 1};
// A=6, 10=5, K=4, Q=3, J=2, 9=1
//-----------------------------------------

bool isTrump(const Card& card, const char* trumpSuit) {
  if (!trumpSuit || trumpSuit[0] == '\0') return false;

  return std::strcmp(card.suit, trumpSuit) == 0;
}

void initializeDeck(Card deck[DECK_MAX_SIZE], int& deckSize) {
  const char* SUITS[] = {"♣", "♦", "♥", "♠"};
  const char* RANKS[] = {"9", "J", "Q", "K", "10", "A"};

  int id = 0;
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 6; i++) {
      std::strncpy(deck[id].suit, SUITS[j], SUIT_MAX_LENGTH);
      deck[id].suit[SUIT_MAX_LENGTH - 1] = '\0';
      std::strncpy(deck[id].rank, RANKS[i], RANK_MAX_LENGTH);
      deck[id].rank[RANK_MAX_LENGTH - 1] = '\0';
      // compute values
      deck[id].suitValue = getSuitValue(deck[id]);
      deck[id].rankValue = getRankValue(deck[id]);
      id++;
    }
  }

  deckSize = DECK_MAX_SIZE;

  // SHUFFLING DECK
  deckShuffle(deck, DECK_MAX_SIZE);
}

void distributeCards(Card deck[DECK_MAX_SIZE], Card P1[], Card P2[], int& deckSize) {
  deckSize = DECK_MAX_SIZE;
  int p1Idx = 0, p2Idx = 0;

  for (int j = 0; j < 2; j++) {
    for (int i = 0; i < 3; i++) {
      // take from the top (deck[DECK_SIZE-1])
      P1[p1Idx++] = deck[--deckSize];
    }
    for (int i = 0; i < 3; i++) {
      P2[p2Idx++] = deck[--deckSize];
    }
  }

  // Sorting hands:
  deckSort(P1, HAND_MAX_SIZE);
  deckSort(P2, HAND_MAX_SIZE);
}

void deckShuffle(Card deck[DECK_MAX_SIZE], int deckSize) {
  for (int i = 0; i < deckSize; i++) {
    int randomIndex = std::rand() % deckSize;
    std::swap(deck[i], deck[randomIndex]);
  }
}

void deckSort(Card deck[DECK_MAX_SIZE], int deckSize) {
  for (int i = 0; i < deckSize - 1; i++) {
    for (int j = 0; j < deckSize - 1 - i; j++)  // o p t i m i z a t i o n
    {
      if (compareCards(deck[j + 1], deck[j])) {
        std::swap(deck[j], deck[j + 1]);
      }
    }
  }
}

void revealTrump(Card deck[DECK_MAX_SIZE], int& deckSize, char* trumpSuit) {
  Card& topCard = deck[deckSize - 1];
  // shift up by one
  for (int i = DECK_MAX_SIZE - 1; i >= 1; i--) {
    deck[i] = deck[i - 1];
    if (i == 1) break;
  }
  deck[0] = topCard;

  // Set trump suit
  std::strncpy(trumpSuit, deck[0].suit, SUIT_MAX_LENGTH);
  trumpSuit[SUIT_MAX_LENGTH - 1] = '\0';
}

int getSuitValue(const Card& card) {
  const char* SUIT_TO_FIND = card.suit;

  // Loop for 4 suits
  for (int i = 0; i < 4; i++) {
    char currentSuitChar[SUIT_MAX_LENGTH];
    std::strncpy(currentSuitChar, &SUIT_CHARS[i * 3], 3);
    currentSuitChar[3] = '\0';  // Ensure null termination

    if (std::strcmp(currentSuitChar, SUIT_TO_FIND) == 0) {
      return SUIT_VALUES[i];
    }
  }
  return -1;
}

int getRankValue(const Card& card) {
  char rankCharToFind = card.rank[0];

  const char* FOUND_PTR = std::strchr(RANK_CHARS, rankCharToFind);

  if (FOUND_PTR != nullptr) {
    return RANK_VALUES[FOUND_PTR - RANK_CHARS];
  }
  return -1;  // Rank not found or invalid
}

bool compareCards(const Card& card1, const Card& card2) {
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

void printRulesString(const int requiredPointsToWin, const int nonTrumpMarriage, const int trumpMarriage) {
  std::cout << "------------ SANTASE GAME RULES ------------\n";
  std::cout << "* Each player gets 6 cards. The trump suit is the suit of the next card in the deck.\n";
  std::cout << "This card gets put below the deck.\n";
  std::cout << "* Card values: A=11, 10=10, K=4, Q=3, J=2, 9=0.\n";
  std::cout << "* A marriage (K+Q of the same suit) gives " << nonTrumpMarriage << " points, or " << trumpMarriage
            << " if of the trump suit.\n";
  std::cout << "* The winner of a trick leads next and draws a card from the stock.\n";
  std::cout << "* Players must follow suit and trump only after the stock has been closed.\n";
  std::cout << "* The first player to reach 66 points wins the round.\n";
  std::cout << "* The game finishes when a player hits " << requiredPointsToWin << " match points.\n";
}

const char* getSuit(const Card& card) {
  if (std::strcmp(card.suit, "♥") == 0)
    return HEARTS;
  else if (std::strcmp(card.suit, "♦") == 0)
    return DIAMONDS;
  else if (std::strcmp(card.suit, "♣") == 0)
    return CLUBS;
  else if (std::strcmp(card.suit, "♠") == 0)
    return SPADES;

  return card.suit;
}

/*
-2 = full hand
-1 = empty deck
 0 = deal completed
*/
int dealCard(Card deck[DECK_MAX_SIZE], Card PHand[], int& deckSize, int& handSize) {
  if (deckSize <= 0) {
    return -1;  // Empty deck
  }

  if (handSize >= HAND_MAX_SIZE) {
    return -2;
  }

  PHand[handSize] = deck[deckSize - 1];
  handSize++;
  deckSize--;

  deckSort(PHand, handSize);

  return 0;
}

void printSuitColored(const char* suit) {
  // COLOR_RED and COLOR_BLACK are used here. Assumes they are externally declared
  const char* COLOR_CODE = (std::strcmp(suit, "♥") == 0 || std::strcmp(suit, "♦") == 0) ? COLOR_RED : COLOR_BLACK;
  std::cout << COLOR_CODE << suit << COLOR_RESET;
}

void printYellowWordTrump() {
  // COLOR_YELLOW is used here. Assumes it's externally declared
  std::cout << COLOR_YELLOW << "Trump" << COLOR_RESET;
}

void printCardIndexes(Card hand[HAND_MAX_SIZE], int handSize, const char* trumpSuit) {
  if (handSize > 0) {
    std::cout << "       ";  // 7x spaces
    for (int i = 0; i < handSize; i++) {
      if (strcmp(hand[i].rank, "10") == 0) {
        // adding one more space for the 10s
        std::cout << " ";
      }

      // COLOR_YELLOW is used here. Assumes it's externally declared
      if (isTrump(hand[i], trumpSuit)) {
        std::cout << COLOR_YELLOW << "[" << i << "] " << COLOR_RESET;
      } else {
        std::cout << "[" << i << "] ";
      }
    }
    std::cout << std::endl;
  }
}

void cardPrint(const Card& card) {
  // COLOR_RED and COLOR_BLACK are used here. Assumes they are externally declared
  const char* COLOR_CODE = (card.suitValue == 3 || card.suitValue == 2) ? COLOR_RED : COLOR_BLACK;
  std::cout << card.rank << COLOR_CODE << getSuit(card) << COLOR_RESET;
}

void printPlayerHand(const Card hand[HAND_MAX_SIZE], size_t size) {
  std::cout << "[ ";

  for (size_t i = 0; i < size; i++) {
    cardPrint(hand[i]);

    if (i != size - 1) std::cout << ", ";
  }
  std::cout << " ]";
}

void printHistory(const GameHistory& game) {
  std::cout << "History for the last " << game.totalRounds << (game.totalRounds == 1 ? " round:" : " rounds:") << std::endl;
  for (int i = 0; i < game.totalRounds; i++) {
    const RoundHistory& round = game.history[i];

    std::cout << "Round " << round.roundNumber << ": ";

    if (round.isOngoing) {
      std::cout << "Ongoing" << std::endl;
    } else {
      std::cout << "Winner - Player " << round.winnerId << " (+" << round.gamePointsWon << ") | "
                << "Player 1: " << round.p1Score << " points | "
                << "Player 2: " << round.p2Score << " points" << std::endl;
    }
  }
  std::cout << "Overall: Player 1 - " << game.overallP1 << " | Player 2 - " << game.overallP2 << std::endl;
}

bool isValidPlayWhenClosed(const Card* playerHand, int playerHandSize, const Card& opponentCard, const char* trumpSuit,
                           int playedIndex) {
  const char* OPPONENT_SUIT = getSuit(opponentCard);

  int validIndexes[HAND_MAX_SIZE];  // Use HAND_MAX_SIZE or a dynamically allocated array
  int countOfValidIndexes = 0;

  bool playerHasMatchingOpponentSuit = false;
  bool handMatchingOpponentSuit[HAND_MAX_SIZE] = {false};

  for (int i = 0; i < playerHandSize; i++) {
    if (strcmp(OPPONENT_SUIT, getSuit(playerHand[i])) == 0) {
      handMatchingOpponentSuit[i] = true;
      playerHasMatchingOpponentSuit = true;
    }
  }

  if (playerHasMatchingOpponentSuit) {
    bool playerHasStrongerCard = false;
    for (int k = 0; k < playerHandSize; k++) {
      if (handMatchingOpponentSuit[k] && !compareCards(opponentCard, playerHand[k])) {  // If a matching suit card is stronger
        validIndexes[countOfValidIndexes++] = k;
        playerHasStrongerCard = true;
      }
    }

    if (!playerHasStrongerCard) {
      for (int k = 0; k < playerHandSize; k++) {
        if (handMatchingOpponentSuit[k]) {  // All matching suit cards are valid if no stronger matching suit card
          validIndexes[countOfValidIndexes++] = k;
        }
      }
    }
  } else {  // Player has no matching suit cards
    bool playerHasTrumpCard = false;
    bool handTrumpCards[HAND_MAX_SIZE] = {false};

    for (int i = 0; i < playerHandSize; i++) {
      if (isTrump(playerHand[i], trumpSuit)) {
        handTrumpCards[i] = true;
        playerHasTrumpCard = true;
      }
    }

    if (playerHasTrumpCard) {
      // Only trump cards are valid
      for (int k = 0; k < playerHandSize; k++) {
        if (handTrumpCards[k]) {
          validIndexes[countOfValidIndexes++] = k;
        }
      }
    } else {
      // Everything is valid
      for (int k = 0; k < playerHandSize; k++) {
        validIndexes[countOfValidIndexes++] = k;
      }
    }
  }

  bool isIndexValid = false;
  for (int i = 0; i < countOfValidIndexes; i++) {
    if (validIndexes[i] == playedIndex) {
      isIndexValid = true;
      break;
    }
  }
  return isIndexValid;
}

bool processPlayerCardPlay(Card playerHand[], int& playerHandSize, Card thrownCards[], int& thrownCount, const char trumpSuit[],
                           int cardIndex, int playerId, bool isRoundClosed) {
  // Check if second thrown card is valid when round is closed
  if (thrownCount == 1 && isRoundClosed) {
    if (!isValidPlayWhenClosed(playerHand, playerHandSize, thrownCards[0], trumpSuit, cardIndex)) {
      std::cout << "You can't play this card!" << std::endl;
      return false;
    }
  }

  thrownCards[thrownCount++] = playerHand[cardIndex];
  std::cout << "P" << playerId << " played ";
  cardPrint(playerHand[cardIndex]);
  std::cout << std::endl;

  // Remove the card from the player's hand
  for (int k = cardIndex; k < playerHandSize - 1; ++k) {
    playerHand[k] = playerHand[k + 1];
  }
  --playerHandSize;
  return true;
}

int roundEnd(bool manualStopCall, int lastTrickWinnerId, int& P1RoundPoints, int& P2RoundPoints, bool P1hasWonCard,
             bool P2hasWonCard, int& P1GamePoints, int& P2GamePoints) {
  if (!manualStopCall) {
    if (lastTrickWinnerId == 1)
      P1RoundPoints += 10;
    else if (lastTrickWinnerId == 2)
      P2RoundPoints += 10;
    std::cout << "Last trick bonus applied (10 points)." << std::endl;
  }

  int wonPoints;
  int roundWinnerID;

  if (P1RoundPoints == P2RoundPoints) {
    std::cout << "Both players have the same number of points (" << P1RoundPoints
              << "). It's a draw for the round! No game points awarded." << std::endl;
    return lastTrickWinnerId;
  }

  int winnerScore, loserScore;
  bool loserHadWonAnyCards;

  if (lastTrickWinnerId == 1) {
    if (P1RoundPoints >= 66) {
      roundWinnerID = 1;
      winnerScore = P1RoundPoints;
      loserScore = P2RoundPoints;
      loserHadWonAnyCards = P2hasWonCard;
    } else {
      roundWinnerID = 2;
      winnerScore = P2RoundPoints;
      loserScore = P1RoundPoints;
      loserHadWonAnyCards = P1hasWonCard;
    }
  } else {
    if (P2RoundPoints >= 66) {
      roundWinnerID = 2;
      winnerScore = P2RoundPoints;
      loserScore = P1RoundPoints;
      loserHadWonAnyCards = P1hasWonCard;
    } else {
      roundWinnerID = 1;
      winnerScore = P1RoundPoints;
      loserScore = P2RoundPoints;
      loserHadWonAnyCards = P2hasWonCard;
    }
  }

  if (!loserHadWonAnyCards) {
    wonPoints = 3;
  } else if (loserScore <= 32) {
    wonPoints = 2;
  } else {
    wonPoints = 1;
  }

  // Update overall game points
  if (roundWinnerID == 1) {
    P1GamePoints += wonPoints;
  } else {
    P2GamePoints += wonPoints;
  }

  std::cout << "Player " << roundWinnerID << " wins the round! (+" << wonPoints << " game points)" << std::endl;
  std::cout << "Player 1 - " << P1RoundPoints << ", P2 - " << P2RoundPoints << std::endl;

  return roundWinnerID;
}

void startNewRoundHistory(GameHistory& game, int RoundNumber) {
  int& totalRounds = game.totalRounds;

  if (totalRounds >= MAX_ROUNDS) {
    for (int i = 1; i < MAX_ROUNDS; ++i) {
      game.history[i - 1] = game.history[i];
    }

    totalRounds = MAX_ROUNDS - 1;
  }

  RoundHistory newOngoingRound;
  newOngoingRound.roundNumber = RoundNumber;
  newOngoingRound.winnerId = 0;
  newOngoingRound.gamePointsWon = 0;
  newOngoingRound.p1Score = 0;
  newOngoingRound.p2Score = 0;
  newOngoingRound.isOngoing = true;

  game.history[totalRounds] = newOngoingRound;
  totalRounds++;
}

void finalizeCurrentRoundHistory(GameHistory& game, int winnerId, int gamePointsWon, int p1RoundScore, int p2RoundScore) {
  if (game.totalRounds == 0) {
    std::cout << "No round found in history to finalize." << std::endl;
    return;
  }

  // last entry in history should be an ongoing round
  RoundHistory& currentRound = game.history[game.totalRounds - 1];

  currentRound.winnerId = winnerId;
  currentRound.gamePointsWon = gamePointsWon;
  currentRound.p1Score = p1RoundScore;
  currentRound.p2Score = p2RoundScore;
  currentRound.isOngoing = false;

  if (winnerId == 1) {
    game.overallP1 += gamePointsWon;
  } else {
    game.overallP2 += gamePointsWon;
  }
}

bool saveGameState(const char* filename, bool hasGameStarted, bool wereSettingsModified, const char trumpSuit[SUIT_MAX_LENGTH],
                   const Card deck[DECK_MAX_SIZE], int deckSize, int currentRoundNumber, const Card P1Hand[HAND_MAX_SIZE],
                   int P1HandSize, const Card P2Hand[HAND_MAX_SIZE], int P2HandSize,
                   const Card thrownCards[THROWN_CARDS_MAX_NUMBER], int thrownCount, int firstPlayedPlayerId, int P1GamePoints,
                   int P2GamePoints, bool P1hasWonCard, bool P2hasWonCard, int P1RoundPoints, int P2RoundPoints,
                   int currentPlayerId, int lastRoundWonPlayerId, bool isStockClosed,
                   const char declaredMarriageSuit[SUIT_MAX_LENGTH], bool isMarriageDeclaredAndCardMustBePlayed,
                   bool manualStopCall, const Card lastTrickCards[THROWN_CARDS_MAX_NUMBER], int lastTrickWinnerId,
                   const GameHistory& history, int requiredPointsToWin, int nonTrumpMarriage, int trumpMarriage,
                   bool arePointsVisible, bool lastTrickBonus) {
  std::ofstream outFile(filename);  // fstream can take const char* directly
  if (!outFile.is_open()) {
    std::cerr << "Error: Could not open file for saving: " << filename << std::endl;
    return false;  // Return false on error
  }

  // Save simple boolean and int variables
  outFile << hasGameStarted << "\n";
  outFile << wereSettingsModified << "\n";
  outFile << trumpSuit << "\n";

  // Save deck
  outFile << deckSize << "\n";
  for (int i = 0; i < deckSize; ++i) {
    outFile << deck[i].rank << " " << deck[i].suit << "\n";
  }

  outFile << currentRoundNumber << "\n";

  // Save P1Hand
  outFile << P1HandSize << "\n";
  for (int i = 0; i < P1HandSize; ++i) {
    outFile << P1Hand[i].rank << " " << P1Hand[i].suit << "\n";
  }
  // Save P2Hand
  outFile << P2HandSize << "\n";
  for (int i = 0; i < P2HandSize; ++i) {
    outFile << P2Hand[i].rank << " " << P2Hand[i].suit << "\n";
  }

  // Save thrownCards
  outFile << thrownCount << "\n";
  for (int i = 0; i < thrownCount; ++i) {
    outFile << thrownCards[i].rank << " " << thrownCards[i].suit << "\n";
  }

  outFile << firstPlayedPlayerId << "\n";
  outFile << P1GamePoints << "\n";
  outFile << P2GamePoints << "\n";
  outFile << P1hasWonCard << "\n";
  outFile << P2hasWonCard << "\n";
  outFile << P1RoundPoints << "\n";
  outFile << P2RoundPoints << "\n";
  outFile << currentPlayerId << "\n";
  outFile << lastRoundWonPlayerId << "\n";
  outFile << isStockClosed << "\n";
  outFile << declaredMarriageSuit << "\n";
  outFile << isMarriageDeclaredAndCardMustBePlayed << "\n";
  outFile << manualStopCall << "\n";

  // Save lastTrickCards
  // Always save 2 cards, even if not fully populated,
  // as lastTrickWinnerId indicates if valid.
  outFile << lastTrickCards[0].rank << " " << lastTrickCards[0].suit << "\n";
  outFile << lastTrickCards[1].rank << " " << lastTrickCards[1].suit << "\n";
  outFile << lastTrickWinnerId << "\n";
  // Save GameHistory
  outFile << history.totalRounds << "\n";
  outFile << history.overallP1 << "\n";
  outFile << history.overallP2 << "\n";
  for (int i = 0; i < history.totalRounds; ++i) {
    outFile << history.history[i].roundNumber << "\n";
    outFile << history.history[i].winnerId << "\n";
    outFile << history.history[i].gamePointsWon << "\n";
    outFile << history.history[i].p1Score << "\n";
    outFile << history.history[i].p2Score << "\n";
    outFile << history.history[i].isOngoing << "\n";
  }

  // Save settings
  outFile << requiredPointsToWin << "\n";
  outFile << nonTrumpMarriage << "\n";
  outFile << trumpMarriage << "\n";
  outFile << arePointsVisible << "\n";
  outFile << lastTrickBonus << "\n";

  outFile.close();
  return true;
}

// Corrected load game state function (using const char* for filename)
bool loadGameState(const char* filename, bool& hasGameStarted, bool& wereSettingsModified, char trumpSuit[SUIT_MAX_LENGTH],
                   Card deck[DECK_MAX_SIZE], int& deckSize, int& currentRoundNumber, Card P1Hand[HAND_MAX_SIZE], int& P1HandSize,
                   Card P2Hand[HAND_MAX_SIZE], int& P2HandSize, Card thrownCards[THROWN_CARDS_MAX_NUMBER], int& thrownCount,
                   int& firstPlayedPlayerId, int& P1GamePoints, int& P2GamePoints, bool& P1hasWonCard, bool& P2hasWonCard,
                   int& P1RoundPoints, int& P2RoundPoints, int& currentPlayerId, int& lastRoundWonPlayerId, bool& isStockClosed,
                   char declaredMarriageSuit[SUIT_MAX_LENGTH], bool& isMarriageDeclaredAndCardMustBePlayed, bool& manualStopCall,
                   Card lastTrickCards[THROWN_CARDS_MAX_NUMBER], int& lastTrickWinnerId, GameHistory& history,
                   int& requiredPointsToWin, int& nonTrumpMarriage, int& trumpMarriage, bool& arePointsVisible,
                   bool& lastTrickBonus) {
  std::ifstream inFile(filename);  // fstream can take const char* directly
  if (!inFile.is_open()) {
    std::cerr << "Error: Could not open file for loading: " << filename << std::endl;
    return false;  // Return false on error
  }

  // Load simple boolean and int variables
  int boolVal;  // Use int to read bools
  inFile >> boolVal;
  hasGameStarted = boolVal;
  inFile >> boolVal;
  wereSettingsModified = boolVal;
  inFile >> trumpSuit;  // Read char array directly

  // Load deck
  inFile >> deckSize;
  for (int i = 0; i < deckSize; ++i) {
    inFile >> deck[i].rank >> deck[i].suit;
    deck[i].suitValue = getSuitValue(deck[i]);
    deck[i].rankValue = getRankValue(deck[i]);
  }

  inFile >> currentRoundNumber;

  // Load P1Hand
  inFile >> P1HandSize;
  for (int i = 0; i < P1HandSize; ++i) {
    inFile >> P1Hand[i].rank >> P1Hand[i].suit;
    P1Hand[i].suitValue = getSuitValue(P1Hand[i]);
    P1Hand[i].rankValue = getRankValue(P1Hand[i]);
  }
  // Load P2Hand
  inFile >> P2HandSize;
  for (int i = 0; i < P2HandSize; ++i) {
    inFile >> P2Hand[i].rank >> P2Hand[i].suit;
    P2Hand[i].suitValue = getSuitValue(P2Hand[i]);
    P2Hand[i].rankValue = getRankValue(P2Hand[i]);
  }

  // Load thrownCards
  inFile >> thrownCount;
  for (int i = 0; i < thrownCount; ++i) {
    inFile >> thrownCards[i].rank >> thrownCards[i].suit;
    thrownCards[i].suitValue = getSuitValue(thrownCards[i]);
    thrownCards[i].rankValue = getRankValue(thrownCards[i]);
  }

  inFile >> firstPlayedPlayerId;
  inFile >> P1GamePoints;
  inFile >> P2GamePoints;
  inFile >> boolVal;
  P1hasWonCard = boolVal;
  inFile >> boolVal;
  P2hasWonCard = boolVal;
  inFile >> P1RoundPoints;
  inFile >> P2RoundPoints;
  inFile >> currentPlayerId;
  inFile >> lastRoundWonPlayerId;
  inFile >> boolVal;
  isStockClosed = boolVal;
  inFile >> declaredMarriageSuit;
  inFile >> boolVal;
  isMarriageDeclaredAndCardMustBePlayed = boolVal;
  inFile >> boolVal;
  manualStopCall = boolVal;

  // Load lastTrickCards
  inFile >> lastTrickCards[0].rank >> lastTrickCards[0].suit;
  lastTrickCards[0].suitValue = getSuitValue(lastTrickCards[0]);
  lastTrickCards[0].rankValue = getRankValue(lastTrickCards[0]);
  inFile >> lastTrickCards[1].rank >> lastTrickCards[1].suit;
  lastTrickCards[1].suitValue = getSuitValue(lastTrickCards[1]);
  lastTrickCards[1].rankValue = getRankValue(lastTrickCards[1]);
  inFile >> lastTrickWinnerId;

  // Load GameHistory
  history.overallP1 = 0;  // Reset cumulative scores as they are re-calculated from rounds
  history.overallP2 = 0;
  inFile >> history.totalRounds;
  inFile >> history.overallP1;  // Re-load overall scores
  inFile >> history.overallP2;
  for (int i = 0; i < history.totalRounds; ++i) {
    inFile >> history.history[i].roundNumber;
    inFile >> history.history[i].winnerId;
    inFile >> history.history[i].gamePointsWon;
    inFile >> history.history[i].p1Score;
    inFile >> history.history[i].p2Score;
    inFile >> boolVal;
    history.history[i].isOngoing = boolVal;
  }

  // Load settings
  inFile >> requiredPointsToWin;
  inFile >> nonTrumpMarriage;
  inFile >> trumpMarriage;
  inFile >> boolVal;
  arePointsVisible = boolVal;
  inFile >> boolVal;
  lastTrickBonus = boolVal;

  inFile.close();

  // After loading, ensure hands are sorted
  deckSort(P1Hand, P1HandSize);
  deckSort(P2Hand, P2HandSize);

  return true;
}