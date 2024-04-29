#ifndef BLOOM_ENGINE_H
#define BLOOM_ENGINE_H

#include<vector>
#include<iostream>
#include<chrono>

#include "bloom_board.h"


using namespace std;

// Quiescence search function
int stabilizePosition(Board& board, int player, int alpha, int beta, int qdepth, chrono::time_point<chrono::steady_clock> end_point = chrono::time_point<chrono::steady_clock>::max());

// Minimax search with alpha/beta pruning
Move findBestMove(Board& board, int depth, int player, LegalMove previous_move, int alpha, int beta, int quiescence_depth, chrono::time_point<chrono::steady_clock> end_point = chrono::time_point<chrono::steady_clock>::max());

int findBestMoveValue(Board& board, int depth, int player, int alpha, int beta, int quiescence_depth, chrono::time_point<chrono::steady_clock> end_point = chrono::time_point<chrono::steady_clock>::max());

Move quickFindBestMove(Board& board, int depth, int player,int alpha, int beta, int quiescence_depth, chrono::time_point<chrono::steady_clock> end_point = chrono::time_point<chrono::steady_clock>::max());

Move iterativeDeepening(Board& board, int seconds, int player, int qdepth);

Move quickIterativeDeepening(Board& board, int seconds, int player, int qdepth);

Move printIterativeDeepening(Board& board, int seconds, int player, int qdepth);

#endif