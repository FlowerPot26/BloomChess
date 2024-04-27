//header file for basic utility functions and constant values used by bloom

#ifndef BOARD_UTIL_H
#define BOARD_UTIL_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <array>

using namespace std;



// Function to convert (x, y) coordinates to array index
int xy_to_i(int x, int y);

// Function to convert array index to (x, y) coordinates
pair<int, int> i_to_xy(int i);

// Function to check if (x, y) coordinates are valid on a 8x8 board
bool checkValidXY(int x, int y);

// Function to convert array index to string representation (e.g., "a1")
string i_to_string(int i);

// Function to convert string representation (e.g., "a1") to array index
int string_to_i(string input_string);

// Constant Values
namespace utility{
    extern const array<pair<int, int>, 8> knight_moves;
    extern const array<pair<int, int>, 4> orthogonal_moves;
    extern const array<pair<int, int>, 4> diagonal_moves;
    extern const vector<int> white_kingside_castle_squares;
    extern const vector<int> white_queenside_castle_squares;
    extern const vector<int> black_kingside_castle_squares ;
    extern const vector<int> black_queenside_castle_squares;
}

// Piece-Square maps for evaluating positions
namespace maps {
    extern const array<int, 64> white_pawn_map;
    extern const array<int, 64> black_pawn_map;
    extern const array<int, 64> white_knight_map;
    extern const array<int, 64> black_knight_map;
    extern const array<int, 64> white_bishop_map;
    extern const array<int, 64> black_bishop_map;
    extern const array<int, 64> white_rook_map;
    extern const array<int, 64> black_rook_map;
    extern const array<int, 64> white_queen_map;
    extern const array<int, 64> black_queen_map;
    extern const array<int, 64> white_king_map;
    extern const array<int, 64> white_king_map_endgame;
    extern const array<int, 64> black_king_map;
    extern const array<int, 64> black_king_map_endgame;
}

#endif // BOARD_UTILS_H

