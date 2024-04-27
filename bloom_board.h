// Header file for Board and Move representation
// Header file for transposition table  

#ifndef BLOOM_BOARD_H
#define BLOOM_BOARD_H


#include<vector>
#include<iostream>
#include<array>
#include<algorithm>
#include "bloom_util.h"
#include "bloom_zobrist.h"

using namespace std;

struct LegalMove {
    int from_i;
    int to_i;
    bool take = false;
    int priority = 0;
};

struct Move {
    int from_i;
    int to_i;
    int player = 0;
    
    int valuation;
    vector<LegalMove> move_stack;
};

struct TranspositionEntry {
    int depth;
    LegalMove best_move;
    int valuation;
    unsigned long long key = 0;
};


struct TranspositionTable {
    LegalMove empty_legal_move = {0, 0};
    TranspositionEntry empty_entry = {0, empty_legal_move, 0, 0};
    vector<TranspositionEntry> table;
    int table_size;
    int replacements = 0;

    void store(unsigned long long key, int depth, LegalMove move, int valuation);
    TranspositionEntry retrieve(unsigned long long key);
    void clear();
    void resize(int size);
    void check();
};

struct Board {

    // Bitboard Representation

    long long white_pawns = 0;
    long long white_rooks = 0;
    long long white_knights = 0;
    long long white_bishops = 0;
    long long white_queens = 0;
    long long white_king = 0;

    long long black_pawns = 0;
    long long black_rooks = 0;
    long long black_knights = 0;
    long long black_bishops = 0;
    long long black_queens = 0;
    long long black_king = 0;

    long long castling_rights = 0;

    long en_passant_i = -1; 
    bool white_move = true;

    long long none_type = 0; 

    int table_size = 800000;

    TranspositionTable transposition_table;

    ZobristKeyStorage zobrist_key_storage;

    unsigned long long zobrist_hash = generateZobristHash(zobrist_key_storage);
    
    // Board constructor
    Board(string fen_string, int size = 800000);

    // Function for Move ordering
    void addLegalMove(vector<LegalMove>& legal_moves, int from_i, int to_i);

    // Move generation for the quiescence search
    vector<LegalMove> generateLegalTakes(int player);

    // Move generation
    vector<LegalMove> generateLegalMoves(int player, bool only_takes, LegalMove table_move = {0, 0});

    // Function to retrieve bitboard representation of the piece-type on the i'th square
    long long& getPieceType(int i);

    // Function for retrieving the piece value on the i'th square, used for move ordering
    int getPieceTypeWorth(int i);

    // Function for retrieving the index of a piece on the i'th square
    int getPieceTypeIndex(int i);

    // Check if a square is attacked
    bool squareUnderAttack(int i, int attacking_player) const;

    // Check if a player is in check
    int checkForChecks();

    // Check if game is over
    int checkGameEnd();

    // Remove a piece from a square and update zobrist hash
    void remove(long long& piece_type, int i, int type_index = -1);

    // Add a piece to a square and update zobrist hash
    void add(long long& piece_type, int i, int type_index = -1);

    // Castling logic
    void castle(int type, bool reversed);

    // Promotion logic
    void promotePawn(LegalMove move, long long& replace_type, bool reversed, bool take = false);

    // En passant logic
    void enPassant(int colour, LegalMove move, bool reversed);

    // Make a move on the board
    void makeMove(LegalMove move, long long&type, long long& replace_type, int type_index, int replace_type_index);

    // Reverse a move on the board
    void reverseMove(LegalMove move, long long&type, long long& replace_type, long long castling_rights_copy, int en_passant_i_copy, unsigned long long hash_copy,  bool take = false);

    // Generate Zobrist hash for the board
    unsigned long long generateZobristHash(ZobristKeyStorage& zobrist_key_storage);

    // Function for checking Zobrist hash
    void printZobristHash(ZobristKeyStorage& zobrist_key_storage);

    void printBoard(int highlight_move = -1);

    int evaluate();

    int simpleEvaluate();

    int countMaterial();
};

#endif