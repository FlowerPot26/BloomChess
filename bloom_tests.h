#ifndef BLOOM_TESTS_H
#define BLOOM_TESTS_H

#include<vector>
#include<iostream>

#include "bloom_board.h"
#include "bloom_engine.h"


using namespace std;

// Input a move to make
void makePlayerMove(Board& board, int player, bool reverse = false);

// Bot move with specified depth
void makeBotMove(Board& board, int player, int depth, int quiescence_depth);

// Bot move with specified time_limit
void makeTimedBotMove(Board& board, int player, int seconds, int qdepth = 10);

// Check Move generation
void printLegalMoves(Board& board, int player, bool only_takes,  LegalMove table_move = {0, 0});

// Check quiescence Move generation
void printLegalTakes(Board& board, int player, LegalMove table_move = {0, 0});

// Simulate game agains bot with specified depth
void simulateGame(int player_side, Board& board, int depth, int quiescence_depth);

// Simulate game against bot with specified time limit
void simulateTimedGame(int player_side, Board& board, int seconds);

// Simulate bot vs bot game with specified depths
void simulateBotGame(Board& board, int depth1, int quiescence_depth1, int depth2, int quiescence_depth2);

// Simulate bot vs bot game with specified time limit
void simulateTimedBotGame(Board& board, int time1, bool quiescence1, int time2, bool quiescence2);

// Measure evaluation speed for 1000 evaluations
void measureEvalSpeed(Board& board);

// Minimax with output
void printFindBestMove(Board& board, int depth, int player, int quiescence_depth);

// Check attack checking
void testAttackSquares(Board& board, int player);

// Check check recognition
void testChecks(Board& board);

// Print board metadata
void printBoardSetup(Board& board);

// Function to check if two zobrist hashes differ by one zobrist key
void findZobristDifference(ZobristKeyStorage& zobrist_key_storage, unsigned long long a, unsigned long long c);

// Print transposition table replacements
void checkReplacements(Board& board);

#endif


