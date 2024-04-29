#include<vector>
#include<iostream>
#include<array>
#include<algorithm>
#include "bloom_util.h"
#include "bloom_zobrist.h"
#include "bloom_board.h"


using namespace std;



void TranspositionTable::store(unsigned long long key, int depth, LegalMove move, int valuation, int alpha, int beta) {
    entries += 1;
    int index = key % table_size;
    if (table[index].key != 0) {
        replacements += 1;
    }
    table[index] = {depth, move, valuation, key, alpha, beta};
    //cout << "Stored data \n";
}

TranspositionEntry TranspositionTable::retrieve(unsigned long long key) {
    
    int index = key % table_size;
    TranspositionEntry entry = table[index];
    if (entry.key == 0) {
        return entry;
    } else {
        if (entry.key == key) {
            return entry;
        } else {
            return empty_entry;
        }
    }
    
}

void TranspositionTable::clear() {
    table.clear();
    replacements = 0;
    entries = 0;
}

void TranspositionTable::resize(int size) {
    table.clear();
    replacements = 0;
    table_size = size;
    table.resize(table_size);
}

void TranspositionTable::check() {
    
    int counter = 0;
    for (int i = 0; i < table_size; i++) {
        TranspositionEntry entry = table[i];
        if (entry.key != 0) {
            counter += 1;
        }
    }
    cout << counter << " of " << table_size << " transposition table slots filled: " << double(counter) / table_size * 100 << " % \n";
    cout << replacements << " entries were replaced and " << entries << " entries stored since the last reset \n";
}


Board::Board(string fen_string, int size) {
    int it = 0;
    int y = 0;
    for (int i = 0; i < fen_string.length(); i++){
        if (fen_string[i] == 'P') {
            white_pawns |= (1LL << it);
        } else if (fen_string[i] == 'R') {
            
            white_rooks |= (1LL << it);
        } else if (fen_string[i] == 'N') {
            
            white_knights |= (1LL << it);
        } else if (fen_string[i] == 'B') {
            white_bishops |= (1LL << it);
        } else if (fen_string[i] == 'Q') {
            white_queens |= (1LL << it);
        } else if (fen_string[i] == 'K') {
            white_king |= (1LL << it);
        } else if (fen_string[i] == 'p') {
            black_pawns |= (1LL << it);
        } else if (fen_string[i] == 'r') {
            black_rooks |= (1LL << it);
        } else if (fen_string[i] == 'n') {
            black_knights |= (1LL << it);
        } else if (fen_string[i] == 'b') {
            black_bishops |= (1LL << it);
        } else if (fen_string[i] == 'q') {
            black_queens |= (1LL << it);
        } else if (fen_string[i] == 'k') {
            black_king |= (1LL << it);
        } else if (fen_string[i] == '/') {
            // Skip to the next rank when encountering '/'
            it -= 1; // Reset 'it' to -1 because it will be incremented below
        } else if (isdigit(fen_string[i])) {
            // Skip the specified number of squares
            it += (fen_string[i] - '0') - 1;
        } else if (fen_string[i] == ' ') {
            if (fen_string[i+1] == 'b') {
                white_move = false;
            }
            i+= 3;
            if (fen_string[i] == '-') {
                i += 1;
            } else {
                while (fen_string[i] != ' ') {
                if (fen_string[i] == 'K') {
                    castling_rights |= (1LL << 0);
                }
                if (fen_string[i] == 'Q') {
                    castling_rights |= (1LL << 1);
                }
                if (fen_string[i] == 'k') {
                    castling_rights |= (1LL << 2);
                }
                if (fen_string[i] == 'q') {
                    castling_rights |= (1LL << 3);
                }
                i++;
                }
            }
            
            i += 1;
            if (fen_string[i] == '-') {
                break;
            } else {
                en_passant_i = string_to_i(fen_string.substr(i, 2));
                break;
            }

        }
        it++;
    }
    

    table_size = size;

    transposition_table.resize(table_size);
    
    zobrist_hash = generateZobristHash(zobrist_key_storage);

}



void Board::addLegalMove(vector<LegalMove>& legal_moves, int from_i, int to_i) {
    bool take = false;

    if (getPieceTypeWorth(to_i) != 0) {
        take = true;
    }

    LegalMove legal_move = {from_i, to_i, take, max((10* getPieceTypeWorth(to_i) - (getPieceTypeWorth(from_i))), 0)};

    legal_moves.push_back(legal_move);
}

vector<LegalMove> Board::generateLegalTakes(int player) {

    int x = 0;
    int y = 0;

    
    vector<LegalMove> legal_moves;


    long long all_pieces   = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king | black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;
    long long white_pieces = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king;
    long long black_pieces = black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;

    if (player == 1) {

        for (int i = 0; i < 64; i++) {

            if (white_pieces & (1LL << i)) {    
                if (white_pawns & (1LL << i)) {
                    
                    if (i >= 8) {
                        // up left
                        if (i % 8 != 0) {
                            if (black_pieces & (1LL << i-9)) {
                                addLegalMove(legal_moves,    i, i-9);
                            } else if (en_passant_i == i-9 && i < 32) {
                                addLegalMove(legal_moves,    i, i-9);
                            }
                        }
                        // up right
                        if (i % 8 != 7) {
                            if (black_pieces & (1LL << i-7)) {
                                addLegalMove(legal_moves, i , i-7);
                            } else if (en_passant_i == i-7 && i < 32 ) {
                                addLegalMove(legal_moves,   i, i-7);
                            }
                        }
                    }
                    
                } else if (white_rooks & (1LL << i)) {
                    //up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                }
                else if (white_knights & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    if ((x - 1 >= 0) && (y - 2 >= 0) && (black_pieces & (1LL << xy_to_i(x-1, y-2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x-1, y-2));
                    }
                    if ((x - 2 >= 0) && (y - 1 >= 0) && (black_pieces & (1LL << xy_to_i(x-2, y-1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x-2, y-1));
                    }
                    if ((x - 2 >= 0) && (y + 1 < 8) && (black_pieces & (1LL << xy_to_i(x-2, y+1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x-2, y+1));
                    }
                    if ((x - 1 >= 0) && (y + 2 < 8) && (black_pieces & (1LL << xy_to_i(x- 1, y + 2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x- 1, y + 2));
                    }

                    if ((x + 1 < 8) && (y - 2 >= 0) && (black_pieces & (1LL << xy_to_i(x+1, y-2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+1, y-2));
                    }
                    if ((x + 2 < 8) && (y - 1 >= 0) && (black_pieces & (1LL << xy_to_i(x+2, y-1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+2, y-1));
                    }
                    if ((x + 2 < 8) && (y + 1 < 8) && (black_pieces & (1LL << xy_to_i(x+2, y+1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+2, y+1));
                    }
                    if ((x + 1 < 8) && (y + 2 < 8) && (black_pieces & (1LL << xy_to_i(x+ 1, y + 2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+ 1, y + 2));
                    }
                } else if (white_bishops & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    
                    
                } else if (white_queens & (1LL << i)) {

                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    
                    

                } else if (white_king & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    for (int x_a:{-1,0,1}) {
                        for (int y_a:{-1,0,1}) {
                            if (x + x_a >= 0 && x + x_a < 8 && y + y_a >= 0 && y + y_a <8) {
                                if ((black_pieces & (1LL << xy_to_i(x + x_a, y + y_a))) && !(squareUnderAttack(xy_to_i(x + x_a, y + y_a), -1))) {
                                    addLegalMove(legal_moves, i, xy_to_i(x + x_a, y + y_a));
                                }
                            }
                        }
                    }
                }
            }
        }

    } else { // blacks turn
        for (int i = 0; i < 64; i++) {
            if (black_pieces & (1LL << i)) {

                if (black_pawns & (1LL << i)) {
                    
                    if (i < 56) {
                        // down left
                        if (i % 8 != 0) {
                            if (white_pieces & (1LL << i+7)) {
                                addLegalMove(legal_moves, i, i+7);
                            } else if (en_passant_i == i+7 && i > 32) {
                                addLegalMove(legal_moves,   i, i+7);
                            }
                        }
                        // down right
                        if (i % 8 != 7) {
                            if (white_pieces & (1LL << i+9)) {
                                addLegalMove(legal_moves, i, i+9);
                            } else if (en_passant_i == i+9 && i > 32) {
                                addLegalMove(legal_moves,   i, i+9);
                            }
                        }
                    }
                } else if (black_rooks & (1LL << i)) {
                    //up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                } else if (black_knights & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    if ((x - 1 >= 0) && (y - 2 >= 0) && (white_pieces & (1LL << xy_to_i(x-1, y-2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x-1, y-2));
                    }
                    if ((x - 2 >= 0) && (y - 1 >= 0) && (white_pieces & (1LL << xy_to_i(x-2, y-1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x-2, y-1));
                    }
                    if ((x - 2 >= 0) && (y + 1 < 8) && (white_pieces & (1LL << xy_to_i(x-2, y+1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x-2, y+1));
                    }
                    if ((x - 1 >= 0) && (y + 2 < 8) && (white_pieces & (1LL << xy_to_i(x- 1, y + 2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x- 1, y + 2));
                    }

                    if ((x + 1 < 8) && (y - 2 >= 0) && (white_pieces & (1LL << xy_to_i(x+1, y-2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+1, y-2));
                    }
                    if ((x + 2 < 8) && (y - 1 >= 0) && (white_pieces & (1LL << xy_to_i(x+2, y-1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+2, y-1));
                    }
                    if ((x + 2 < 8) && (y + 1 < 8) && (white_pieces & (1LL << xy_to_i(x+2, y+1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+2, y+1));
                    }
                    if ((x + 1 < 8) && (y + 2 < 8) && (white_pieces & (1LL << xy_to_i(x+ 1, y + 2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+ 1, y + 2));
                    }
                } 
                else if (black_bishops & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    
                    
                } else if (black_queens & (1LL << i)) {

                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        }
                    }
                    // up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        }
                    }
                    
                    

                } else if (black_king & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    for (int x_a:{-1,0,1}) {
                        for (int y_a:{-1,0,1}) {
                            if (x + x_a >= 0 && x + x_a < 8 && y + y_a >= 0 && y + y_a <8) {
                                if ((white_pieces & (1LL << xy_to_i(x + x_a, y + y_a))) && !(squareUnderAttack(xy_to_i(x + x_a, y + y_a), 1))) {
                                    addLegalMove(legal_moves,  i, xy_to_i(x + x_a, y + y_a));
                                }
                            }
                        }
                    }
                }
                
            }
        }
    }

    sort(legal_moves.begin(), legal_moves.end(), [](const LegalMove& a, const LegalMove& b) {
        return a.priority > b.priority;
    });

    return legal_moves;

}


vector<LegalMove> Board::generateLegalMoves(int player, bool only_takes, LegalMove table_move) {

    int x = 0;
    int y = 0;

    
    vector<LegalMove> legal_moves;


    long long all_pieces   = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king | black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;
    long long white_pieces = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king;
    long long black_pieces = black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;

    if (player == 1) {

        for (int i = 0; i < 64; i++) {

            if (white_pieces & (1LL << i)) {    
                if (white_pawns & (1LL << i)) {
                    
                    if (i >= 8) {
                        // up left
                        if (i % 8 != 0) {
                            if (black_pieces & (1LL << i-9)) {
                                addLegalMove(legal_moves,    i, i-9);
                            } else if (en_passant_i == i-9 && i < 32) {
                                addLegalMove(legal_moves,    i, i-9);
                            }
                        }
                        // up right
                        if (i % 8 != 7) {
                            if (black_pieces & (1LL << i-7)) {
                                addLegalMove(legal_moves, i , i-7);
                            } else if (en_passant_i == i-7 && i < 32 ) {
                                addLegalMove(legal_moves,   i, i-7);
                            }
                        }
                        // up 1
                        if (!(white_pieces & (1LL << i-8)) && !(black_pieces & (1LL << i-8))) {
                            addLegalMove(legal_moves ,i, i-8);
                            if (i /8 == 6) {
                                if (!(white_pieces & (1LL << i-16)) && !(black_pieces & (1LL << i-16))) {
                                    addLegalMove(legal_moves,i, i-16);
                                }
                            }
                            
                        }
                    }
                    
                } else if (white_rooks & (1LL << i)) {
                    //up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                }
                else if (white_knights & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    if ((x - 1 >= 0) && (y - 2 >= 0) && !(white_pieces & (1LL << xy_to_i(x-1, y-2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x-1, y-2));
                    }
                    if ((x - 2 >= 0) && (y - 1 >= 0) && !(white_pieces & (1LL << xy_to_i(x-2, y-1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x-2, y-1));
                    }
                    if ((x - 2 >= 0) && (y + 1 < 8) && !(white_pieces & (1LL << xy_to_i(x-2, y+1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x-2, y+1));
                    }
                    if ((x - 1 >= 0) && (y + 2 < 8) && !(white_pieces & (1LL << xy_to_i(x- 1, y + 2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x- 1, y + 2));
                    }

                    if ((x + 1 < 8) && (y - 2 >= 0) && !(white_pieces & (1LL << xy_to_i(x+1, y-2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+1, y-2));
                    }
                    if ((x + 2 < 8) && (y - 1 >= 0) && !(white_pieces & (1LL << xy_to_i(x+2, y-1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+2, y-1));
                    }
                    if ((x + 2 < 8) && (y + 1 < 8) && !(white_pieces & (1LL << xy_to_i(x+2, y+1)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+2, y+1));
                    }
                    if ((x + 1 < 8) && (y + 2 < 8) && !(white_pieces & (1LL << xy_to_i(x+ 1, y + 2)))) {
                        addLegalMove(legal_moves, i, xy_to_i(x+ 1, y + 2));
                    }
                } else if (white_bishops & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    
                    
                } else if (white_queens & (1LL << i)) {

                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                            break;
                        } else if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, xy_to_i(xi, yi));
                        }
                    }
                    // up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (black_pieces & (1LL << y)) {
                            addLegalMove(legal_moves, i, y);
                            break;
                        } else if (white_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves, i, y);
                        }
                    }
                    
                    

                } else if (white_king & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    for (int x_a:{-1,0,1}) {
                        for (int y_a:{-1,0,1}) {
                            if (x + x_a >= 0 && x + x_a < 8 && y + y_a >= 0 && y + y_a <8) {
                                if (!(white_pieces & (1LL << xy_to_i(x + x_a, y + y_a))) && !(squareUnderAttack(xy_to_i(x + x_a, y + y_a), -1))) {
                                    addLegalMove(legal_moves, i, xy_to_i(x + x_a, y + y_a));
                                }
                            }
                        }
                    }
                    if (i == 60) {
                        // White Kingside Castle
                        if ((castling_rights & (1LL << 0)) != 0) {
                            if ((white_rooks & (1LL << 63)) != 0) {
                                bool castle_possible = true;
                                for (int y:utility::white_kingside_castle_squares) {
                                    if (all_pieces & (1LL << y)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                for (int y = 0; y < 3; y++) {
                                    if (squareUnderAttack(i+y, -1)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                if (castle_possible) {
                                    addLegalMove(legal_moves, i, i+2);
                                }
                                
                            }
                        }
                        // White Queenside Castle
                        if ((castling_rights & (1LL << 1)) != 0) {
                            if ((white_rooks & (1LL << 56)) != 0) {
                                bool castle_possible = true;
                                for (int y:utility::white_queenside_castle_squares) {
                                    if (all_pieces & (1LL << y)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                for (int y = 0; y < 3; y++) {
                                    if (squareUnderAttack(i-y, -1)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                if (castle_possible) {
                                    addLegalMove(legal_moves, i, i-2);
                                }
                                
                            }
                        }
                    }
                }
            }
        }

    } else { // blacks turn
        for (int i = 0; i < 64; i++) {
            if (black_pieces & (1LL << i)) {

                if (black_pawns & (1LL << i)) {
                    
                    if (i < 56) {
                        // down left
                        if (i % 8 != 0) {
                            if (white_pieces & (1LL << i+7)) {
                                addLegalMove(legal_moves, i, i+7);
                            } else if (en_passant_i == i+7 && i > 32) {
                                addLegalMove(legal_moves,   i, i+7);
                            }
                        }
                        // down right
                        if (i % 8 != 7) {
                            if (white_pieces & (1LL << i+9)) {
                                addLegalMove(legal_moves, i, i+9);
                            } else if (en_passant_i == i+9 && i > 32) {
                                addLegalMove(legal_moves,   i, i+9);
                            }
                        }
                        // down 1
                        if (!(black_pieces & (1LL << i+8)) && !(white_pieces & (1LL << i+8))) {
                            addLegalMove(legal_moves, i, i+8);
                            if (i /8 == 1) {
                                if (!(white_pieces & (1LL << i+16)) && !(black_pieces & (1LL << i+16))) {
                                    addLegalMove(legal_moves, i, i+16);
                                }
                            }
                            
                        }
                    }
                } else if (black_rooks & (1LL << i)) {
                    //up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                } else if (black_knights & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    if ((x - 1 >= 0) && (y - 2 >= 0) && !(black_pieces & (1LL << xy_to_i(x-1, y-2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x-1, y-2));
                    }
                    if ((x - 2 >= 0) && (y - 1 >= 0) && !(black_pieces & (1LL << xy_to_i(x-2, y-1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x-2, y-1));
                    }
                    if ((x - 2 >= 0) && (y + 1 < 8) && !(black_pieces & (1LL << xy_to_i(x-2, y+1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x-2, y+1));
                    }
                    if ((x - 1 >= 0) && (y + 2 < 8) && !(black_pieces & (1LL << xy_to_i(x- 1, y + 2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x- 1, y + 2));
                    }

                    if ((x + 1 < 8) && (y - 2 >= 0) && !(black_pieces & (1LL << xy_to_i(x+1, y-2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+1, y-2));
                    }
                    if ((x + 2 < 8) && (y - 1 >= 0) && !(black_pieces & (1LL << xy_to_i(x+2, y-1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+2, y-1));
                    }
                    if ((x + 2 < 8) && (y + 1 < 8) && !(black_pieces & (1LL << xy_to_i(x+2, y+1)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+2, y+1));
                    }
                    if ((x + 1 < 8) && (y + 2 < 8) && !(black_pieces & (1LL << xy_to_i(x+ 1, y + 2)))) {
                        addLegalMove(legal_moves,  i, xy_to_i(x+ 1, y + 2));
                    }
                } 
                else if (black_bishops & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    
                    
                } else if (black_queens & (1LL << i)) {

                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    // bottom right
                    for (int xi = x + 1,yi = y + 1; xi < 8 && yi < 8; xi++, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    // top left
                    for (int xi = x - 1,yi = y - 1; xi >= 0 && yi >=  0; xi--, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    // top right
                    for (int xi = x + 1,yi = y - 1; xi < 8 && yi >=  0; xi++, yi-- ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    // bottom left
                    for (int xi = x - 1,yi = y + 1; xi >= 0 && yi < 8; xi--, yi++ ) {
                        if (white_pieces & (1LL << xy_to_i(xi, yi))) {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                            break;
                        } else if (black_pieces & (1LL << xy_to_i(xi, yi))) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, xy_to_i(xi, yi));
                        }
                    }
                    // up
                    for (int y = i - 8; y >= 0; y-= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                    //down
                    for (int y = i + 8; y < 64; y+= 8) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                    // left
                    for (int y = i - 1; y >= i - (i%8) ; y-= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                    // right
                    for (int y = i + 1; y < i + (8 - (i % 8) ); y+= 1) {
                        if (white_pieces & (1LL << y)) {
                            addLegalMove(legal_moves,  i, y);
                            break;
                        } else if (black_pieces & (1LL << y)) {
                            break;
                        } else {
                            addLegalMove(legal_moves,  i, y);
                        }
                    }
                    
                    

                } else if (black_king & (1LL << i)) {
                    x = i_to_xy(i).first; y = i_to_xy(i).second;
                    for (int x_a:{-1,0,1}) {
                        for (int y_a:{-1,0,1}) {
                            if (x + x_a >= 0 && x + x_a < 8 && y + y_a >= 0 && y + y_a <8) {
                                if (!(black_pieces & (1LL << xy_to_i(x + x_a, y + y_a))) && !(squareUnderAttack(xy_to_i(x + x_a, y + y_a), 1))) {
                                    addLegalMove(legal_moves,  i, xy_to_i(x + x_a, y + y_a));
                                }
                            }
                        }
                    }
                    if (i == 4) {
                        // Black Kingside Castle
                        if ((castling_rights & (1LL << 2)) != 0) {
                            if ((black_rooks & (1LL << 7)) != 0) {
                                bool castle_possible = true;
                                for (int y:utility::black_kingside_castle_squares) {
                                    if (all_pieces & (1LL << y)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                for (int y = 0; y < 3; y++) {
                                    if (squareUnderAttack(i+y, 1)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                if (castle_possible) {
                                    addLegalMove(legal_moves, i, i+2);
                                }
                                
                            }
                        }
                        // Black Queenside Castle
                        if ((castling_rights & (1LL << 3)) != 0) {
                            if ((black_rooks & (1LL << 0)) != 0) {
                                bool castle_possible = true;
                                for (int y:utility::black_queenside_castle_squares) {
                                    if (all_pieces & (1LL << y)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                for (int y = 0; y < 3; y++) {
                                    if (squareUnderAttack(i-y, 1)) {
                                        castle_possible = false;
                                        break;
                                    }
                                }
                                if (castle_possible) {
                                    addLegalMove(legal_moves, i, i-2);
                                }
                                
                            }
                        }
                    }
                }
                
            }
        }
    }

    for (int i = 0; i < legal_moves.size(); i++) {
        if (legal_moves[i].to_i == table_move.to_i && legal_moves[i].from_i == table_move.from_i) {
            legal_moves[i].priority = 1000;
            break;
        }
    }

    
    sort(legal_moves.begin(), legal_moves.end(), [](const LegalMove& a, const LegalMove& b) {
        return a.priority > b.priority;
    });
    

    if (only_takes) {
        vector<LegalMove> legal_takes = {};
        for (LegalMove legal_move:legal_moves) {
            if (legal_move.priority != 0) {
                legal_takes.push_back(legal_move);
            }
        }
        return legal_takes;
    }
    else {
        return legal_moves;
    
    }
}

long long& Board::getPieceType(int i) {
    if (white_pawns & (1LL << i)) return white_pawns;
    if (white_rooks & (1LL << i)) return white_rooks;
    if (white_knights & (1LL << i)) return white_knights;
    if (white_bishops & (1LL << i)) return white_bishops;
    if (white_queens & (1LL << i)) return white_queens;
    if (white_king & (1LL << i)) return white_king;
    if (black_pawns & (1LL << i)) return black_pawns;
    if (black_rooks & (1LL << i)) return black_rooks;
    if (black_knights & (1LL << i)) return black_knights;
    if (black_bishops & (1LL << i)) return black_bishops;
    if (black_queens & (1LL << i)) return black_queens;
    if (black_king & (1LL << i)) return black_king;
    
    static long long none = 0;
    return none_type;
}

int Board::getPieceTypeWorth(int i) {
    if (white_pawns & (1LL << i)) return 1;
    if (white_rooks & (1LL << i)) return 4;
    if (white_knights & (1LL << i)) return 2;
    if (white_bishops & (1LL << i)) return 3;
    if (white_queens & (1LL << i)) return 5;
    if (white_king & (1LL << i)) return 6;
    if (black_pawns & (1LL << i)) return 1;
    if (black_rooks & (1LL << i)) return 4;
    if (black_knights & (1LL << i)) return 2;
    if (black_bishops & (1LL << i)) return 3;
    if (black_queens & (1LL << i)) return 5;
    if (black_king & (1LL << i)) return 6;
    
    
    return 0;
}

int Board::getPieceTypeIndex(int i) {
    if (white_pawns & (1LL << i)) return 0;
    if (white_knights & (1LL << i)) return 1;
    if (white_bishops & (1LL << i)) return 2;
    if (white_rooks & (1LL << i)) return 3;
    if (white_queens & (1LL << i)) return 4;
    if (white_king & (1LL << i)) return 5;
    if (black_pawns & (1LL << i)) return 6;
    if (black_knights & (1LL << i)) return 7;
    if (black_bishops & (1LL << i)) return 8;
    if (black_rooks & (1LL << i)) return 9;
    if (black_queens & (1LL << i)) return 10;
    if (black_king & (1LL << i)) return 11;
    return -1;
}



bool Board::squareUnderAttack(int i, int attacking_player) const {

    long long all_pieces = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king | black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;

    
    
    pair<int,int> xy = i_to_xy(i);
    int x = xy.first; int y = xy.second;

    if (attacking_player == 1) {
        if (checkValidXY(x-1, y+1) && white_pawns & (1LL << xy_to_i(x-1, y+1))) {
            return true;
        }
        if (checkValidXY(x+1, y+1) && white_pawns & (1LL << xy_to_i(x-1, y+1))) {
            return true;
        }

        for (pair<int, int> move:utility::knight_moves) {
            if (checkValidXY(x+move.first, y+move.second) && white_knights & (1LL << xy_to_i(x+move.first, y+move.second))) {
            return true;
            }
        }

        for (pair<int, int> move:utility::orthogonal_moves) {
            if (checkValidXY(x+move.first, y+move.second) && white_king & (1LL << xy_to_i(x+move.first, y+move.second))) {
                return true;
            }
            for (int factor = 1; factor < 8; factor++) {
                if (checkValidXY(x + move.first * factor, y+ move.second * factor)) {
                    if (white_rooks & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)) || white_queens & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)))  {
                        return true;
                    } else if (all_pieces & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor))) {
                        break;
                    }
                }
            }
        }

        for (pair<int, int> move:utility::diagonal_moves) {
            if (checkValidXY(x+move.first, y+move.second) && white_king & (1LL << xy_to_i(x+move.first, y+move.second))) {
                return true;
            }
            for (int factor = 1; factor < 8; factor++) {
                if (checkValidXY(x + move.first * factor, y+ move.second * factor)) {
                    if (white_bishops & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)) || white_queens & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)))  {
                        return true;
                    } else if (all_pieces & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor))) {
                        break;
                    }
                }
            }
        }
        

        return false;
    } else {
        if (checkValidXY(x-1, y+1) && black_pawns & (1LL << xy_to_i(x-1, y-1))) {
            return true;
        }
        if (checkValidXY(x+1, y+1) && black_pawns & (1LL << xy_to_i(x-1, y-1))) {
            return true;
        }

        for (pair<int, int> move:utility::knight_moves) {
            if (checkValidXY(x+move.first, y+move.second) && black_knights & (1LL << xy_to_i(x+move.first, y+move.second))) {
            return true;
            }
        }

        for (pair<int, int> move:utility::orthogonal_moves) {
            if (checkValidXY(x+move.first, y+move.second) && black_king & (1LL << xy_to_i(x+move.first, y+move.second))) {
                return true;
            }
            for (int factor = 1; factor < 8; factor++) {
                if (checkValidXY(x + move.first * factor, y+ move.second * factor)) {
                    if (black_rooks & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)) || black_queens & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)))  {
                        return true;
                    } else if (all_pieces & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor))) {
                        break;
                    }
                }
            }
        }

        for (pair<int, int> move:utility::diagonal_moves) {
            if (checkValidXY(x+move.first, y+move.second) && black_king & (1LL << xy_to_i(x+move.first, y+move.second))) {
                return true;
            }
            for (int factor = 1; factor < 8; factor++) {
                if (checkValidXY(x + move.first * factor, y+ move.second * factor)) {
                    if (black_bishops & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)) || black_queens & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor)))  {
                        return true;
                    } else if (all_pieces & (1LL << xy_to_i(x + move.first * factor, y+ move.second * factor))) {
                        break;
                    }
                }
            }
        }

        return false;
    }
}

int Board::checkForChecks() {
    int white_king_index;
    int black_king_index;

    for (int i = 0; i < 64; i++) {
        if (white_king & (1LL << i)) {
            white_king_index = i;
        }
        if (black_king & (1LL << i)) {
            black_king_index = i;
        }
    }

    if (squareUnderAttack(white_king_index, -1)) {
        return 1;
    }
    if (squareUnderAttack(black_king_index, 1)) {
        return -1;
    }
    return 0;
}

int Board::checkGameEnd() {
    if (white_king == 0) {
        return -1;
    } else if (black_king == 0) {
        return 1;
    } else {
        int player_in_check = checkForChecks();
        if (player_in_check == 0) {
            return 0;
        } else {
            vector<LegalMove> legal_moves = generateLegalMoves(player_in_check, false);
            if (legal_moves.size() == 0) {
                return -player_in_check;
            } else {
                return 0;
            }
        }
    }
}

void Board::remove(long long& piece_type, int i, int type_index) {
    //cout << "remove called with index: " << type_index << "\n";
    piece_type &= (~(1LL << i));
    if (type_index != -1) {
        zobrist_hash ^= (zobrist_key_storage.piece_squares[type_index * 64 + i]);
    }
    return;
}

void Board::add(long long& piece_type, int i, int type_index) {
    //cout << "add called with index: " << type_index << "\n";
    piece_type |= (1LL << i);
    if (type_index != -1) {
        zobrist_hash ^= (zobrist_key_storage.piece_squares[type_index * 64 + i]);
    }
    return;
}

void Board::castle(int type, bool reversed) {
    if (type == 0) {
        // White Kingside
        if (reversed) {
            
            remove(white_king, 62, 5);
            add(white_king, 60, 5);

            
            remove(white_rooks, 61, 3);
            add(white_rooks, 63, 3);
        } else {


            
            remove(white_king, 60, 5);
            add(white_king, 62, 5);

            remove(white_rooks, 63, 3);
            add(white_rooks, 61, 3);
        } 
    }
    if (type == 1) {
        //White Queenside
        if (reversed) {
            
            remove(white_king, 58, 5);
            add(white_king, 60, 5);

            remove(white_rooks, 59, 3);
            add(white_rooks, 56, 3);
        } else {
            
            remove(white_king, 60, 5);
            add(white_king, 58, 5);

            remove(white_rooks, 56, 3);
            add(white_rooks, 59, 3);
        } 
    }
    if (type == 2) {
        // Black Kingside
        if (reversed) {
            remove(black_king, 6, 11);
            add(black_king, 4, 11);

            remove(black_rooks, 5, 9);
            add(black_rooks, 7, 9);
        } else {
            remove(black_king, 4, 11);
            add(black_king, 6, 11);

            remove(black_rooks, 7, 9);
            add(black_rooks, 5, 9);
        } 
    }
    if (type == 3) {
        // Black Queenside
        if (reversed) {
            remove(black_king, 2, 11);
            add(black_king, 4, 11);

            remove(black_rooks, 3, 9);
            add(black_rooks, 0, 9);
        } else {
            

            remove(black_king, 4, 11);
            add(black_king, 2, 11);

            remove(black_rooks, 0, 9);
            add(black_rooks, 3, 9);
        } 
    }
}

void Board::promotePawn(LegalMove move, long long& replace_type, bool reversed, bool take) {
    if (!reversed) {
        int replace_index = getPieceTypeIndex(move.to_i);
        if (move.to_i < 8) {
            remove(white_pawns, move.from_i, 0);
            add(white_queens, move.to_i, 4);
            if (replace_type != 0) {
                remove(replace_type, move.to_i, replace_index);
            }
        } else if (move.to_i > 55) {
            remove(black_pawns, move.from_i, 6);
            add(black_queens, move.to_i, 10);
            if (replace_type != 0) {
                remove(replace_type, move.to_i, replace_index);
            }
        } else {
            throw domain_error("cannot promote a Pawn on a non promotable square");
        }
    } else {
        if (move.to_i < 8) {
            add(white_pawns, move.from_i);
            remove(white_queens, move.to_i);
            if (take) {
                add(replace_type, move.to_i);
            }
        } else if (move.to_i > 55) {
            add(black_pawns, move.from_i);
            remove(black_queens, move.to_i);
            if (take) {
                add(replace_type, move.to_i);
            }
        } else {
            throw domain_error("cannot de-promote a Pawn on a non promotable square");
        } 
    }
    
}

void Board::enPassant(int colour, LegalMove move, bool reversed) {
    if (!reversed) {
        if (colour == 1) {
            add(white_pawns, move.to_i, 0);
            remove(white_pawns, move.from_i, 0);
            remove(black_pawns, move.to_i + 8, 6);
        } else {
            add(black_pawns, move.to_i, 6);
            remove(black_pawns, move.from_i, 6);
            remove(white_pawns, move.to_i - 8, 0);
        }
    } else {
        if (colour == 1) {
            remove(white_pawns, move.to_i);
            add(white_pawns, move.from_i);
            add(black_pawns, move.to_i + 8);
        } else {
            remove(black_pawns, move.to_i);
            add(black_pawns, move.from_i);
            add(white_pawns, move.to_i - 8);
        }
    }
    
}

void Board::makeMove(LegalMove move, long long&type, long long& replace_type, int type_index, int replace_type_index) {
    
    //piece handling
    if (type == white_king && move.to_i == 62 && move.from_i == 60) {
        castle(0, false);
    } else if (type == white_king && move.to_i == 58 && move.from_i == 60) {
        castle(1, false);
    } else if (type == black_king && move.to_i == 6 && move.from_i == 4) {
        castle(2, false);
    } else if (type == black_king && move.to_i == 2 && move.from_i == 4) {
        castle(3, false);
    } else if (type == white_pawns  && move.to_i < 8) {
        promotePawn(move, replace_type, false);
    } else if (type == black_pawns && move.to_i > 55) {
        promotePawn(move, replace_type, false);

    } else if (type == white_pawns && move.to_i == en_passant_i && (move.from_i == move.to_i + 7 || move.from_i == move.to_i + 9)) {
        enPassant(1, move, false);
    } else if (type == black_pawns && move.to_i == en_passant_i && (move.from_i == move.to_i - 7 || move.from_i == move.to_i - 9)) {
        enPassant(-1, move, false);
    } else {
        remove(type, move.from_i, type_index);
        add(type, move.to_i, type_index);
        if (replace_type != none_type) {
            remove(replace_type, move.to_i, replace_type_index);
        }
    }

    //permission handling
    if (type == white_king) {
        if (castling_rights & (1LL << 0)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[0];
        }
        remove(castling_rights, 0);
        if (castling_rights & (1LL << 1)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[1];
        }
        remove(castling_rights, 1);
    }

    if (type == black_king) {
        if (castling_rights & (1LL << 2)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[2];
        }
        remove(castling_rights, 2);
        if (castling_rights & (1LL << 3)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[3];
        }
        remove(castling_rights, 3);
    }

    if (type == white_rooks && move.from_i == 63) {
        if (castling_rights & (1LL << 0)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[0];
        }
        remove(castling_rights, 0);
    }
    if (type == white_rooks && move.from_i == 56) {
        if (castling_rights & (1LL << 1)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[1];
        }
        remove(castling_rights, 1);
    }
    if (type == black_rooks && move.from_i == 7) {
        if (castling_rights & (1LL << 2)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[2];
        }
        remove(castling_rights, 2);
    }
    if (type == black_rooks && move.from_i == 0) {
        if (castling_rights & (1LL << 3)) {
            zobrist_hash ^= zobrist_key_storage.castling_rights[3];
        }
        remove(castling_rights, 3);
    }

    if (type == white_pawns && (move.from_i - move.to_i == 16)) {
        if (en_passant_i != -1) {
            zobrist_hash ^= zobrist_key_storage.en_passant_squares[en_passant_i];
        }
        en_passant_i = move.from_i - 8;
        zobrist_hash ^= zobrist_key_storage.en_passant_squares[en_passant_i];
        
    } else if (type == black_pawns && (move.to_i - move.from_i == 16)) {
        if (en_passant_i != -1) {
            zobrist_hash ^= zobrist_key_storage.en_passant_squares[en_passant_i];
        }
        en_passant_i = move.from_i + 8;
        zobrist_hash ^= zobrist_key_storage.en_passant_squares[en_passant_i];
    } else {
        if (en_passant_i != -1) {
            zobrist_hash ^= zobrist_key_storage.en_passant_squares[en_passant_i];
        }
        en_passant_i = -1;
    }

    zobrist_hash ^= zobrist_key_storage.white_turn;
    white_move = !white_move;

}

void Board::reverseMove(LegalMove move, long long&type, long long& replace_type, long long castling_rights_copy, int en_passant_i_copy, unsigned long long hash_copy,  bool take) {

    if (type == white_king && move.to_i == 62 && move.from_i == 60) {
        castle(0, true);
    } else if (type == white_king && move.to_i == 58 && move.from_i == 60) {
        castle(1, true);
    } else if (type == black_king && move.to_i == 6 && move.from_i == 4) {
        castle(2, true);
    } else if (type == black_king && move.to_i == 2 && move.from_i == 4) {
        castle(3, true);
    } else if (type == white_pawns  && move.to_i < 8) {
        promotePawn(move, replace_type, true, take);
    } else if (type == black_pawns && move.to_i > 55) {
        promotePawn(move, replace_type, true, take);
    } else if (type == white_pawns && move.to_i == en_passant_i_copy && (move.from_i == move.to_i + 7 || move.from_i == move.to_i + 9)) {
        enPassant(1, move, true);
    } else if (type == black_pawns && move.to_i == en_passant_i_copy && (move.from_i == move.to_i - 7 || move.from_i == move.to_i - 9)) {
        enPassant(-1, move, true);
    } else {
        remove(type, move.to_i);
        add(type, move.from_i);
        if (take) {
            add(replace_type, move.to_i);
        }
    }

    castling_rights = castling_rights_copy;
    en_passant_i = en_passant_i_copy;
    zobrist_hash = hash_copy;

    white_move = !white_move;
}

unsigned long long Board::generateZobristHash(ZobristKeyStorage& zobrist_key_storage) {
    unsigned long long hash = 0;
    int index = 0;
    for (int i = 0; i< 64; i++) {
        index = getPieceTypeIndex(i);
        if (index != -1) {
            hash ^= zobrist_key_storage.piece_squares[index * 64 + i];
        }
    }
    for (int i = 0; i < 4; i++) {
        if (castling_rights & (1LL << i)) {
            hash ^= zobrist_key_storage.castling_rights[i];
        }
    }
    if (en_passant_i != -1) {
        hash ^= zobrist_key_storage.en_passant_squares[en_passant_i];
    }
    if (white_move) {
        hash ^= zobrist_key_storage.white_turn;
    }

    
    return hash;
}

void Board::printZobristHash(ZobristKeyStorage& zobrist_key_storage) {
    cout << "Board Zobrist Hash: " << zobrist_hash << "\n";
    cout << "Real zobrist Hash:  " << generateZobristHash(zobrist_key_storage) << "\n";
}

void Board::printBoard(int highlight_move) {
    long long all_pieces = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king | black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;

    cout << "----------------------------------------- \n|";
    for (int i = 0; i < 64; i++) {
        if ((all_pieces & 1LL << i) != 0) {
            if ((white_pawns & 1LL << i) != 0) {
                cout << " ♟  |";
            } else if ((white_rooks & 1LL << i) != 0) {
                cout << " ♜  |";
            } else if ((white_knights & 1LL << i) != 0) {
                cout << " ♞  |";
            } else if ((white_bishops & 1LL << i) != 0) {
                cout << " ♝  |";
            } else if ((white_queens & 1LL << i) != 0) {
                cout << " ♛  |";
            } else if ((white_king & 1LL << i) != 0) {
                cout << " ♚  |";
            } else if ((black_pawns & 1LL << i) != 0) {
                cout << " ♙  |";
            } else if ((black_rooks & 1LL << i) != 0) {
                cout << " ♖  |";
            } else if ((black_knights & 1LL << i) != 0) {
                cout << " ♘  |";
            } else if ((black_bishops & 1LL << i) != 0) {
                cout << " ♗  |";
            } else if ((black_queens & 1LL << i) != 0) {
                cout << " ♕  |";
            } else if ((black_king & 1LL << i) != 0) {
                cout << " ♔  |";
            } else if (i == highlight_move) {
                cout << "  x  |";
            }
        } else {
            cout << "    |";
        }
        if ((i + 1) % 8 == 0) {
                cout << "\n----------------------------------------- \n";
            }
    }
}

int Board::evaluate() {
    

    long long white_pieces = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king;
    long long black_pieces = black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;
    int value = 0;
    
    for (int i = 0; i < 64; i++) {
        if ((white_pieces & 1LL << i) != 0) {
            if ((white_pawns & 1LL << i) != 0) {
                value += 100 + maps::white_pawn_map[i];
            } else if ((white_rooks & 1LL << i) != 0) {
                value += 500 + maps::white_rook_map[i];
            } else if ((white_knights & 1LL << i) != 0) {
                value += 300 + maps::white_knight_map[i];
            } else if ((white_bishops & 1LL << i) != 0) {
                value += 300 + maps::white_bishop_map[i];
            } else if ((white_queens & 1LL << i) != 0) {
                value += 900 + maps::white_queen_map[i];
            } else if ((white_king & 1LL << i) != 0) {
                value += 100000 + maps::white_king_map[i];
            } 
        } else if  ((black_pieces & 1LL << i) != 0) {
            if ((black_pawns & 1LL << i) != 0) {
                value -= 100 + maps::black_pawn_map[i];
            } else if ((black_rooks & 1LL << i) != 0) {
                value -= 500 + maps::black_rook_map[i];
            } else if ((black_knights & 1LL << i) != 0) {
                value -= 300 + maps::black_knight_map[i];
            } else if ((black_bishops & 1LL << i) != 0) {
                value -= 300 + maps::black_bishop_map[i];
            } else if ((black_queens & 1LL << i) != 0) {
                value -= 900 + maps::black_queen_map[i];
            } else if ((black_king & 1LL << i) != 0) {
                value -= 100000 + maps::black_king_map[i];
            }
        }
    }

    return value;
}

int Board::simpleEvaluate() {
    

    long long white_pieces = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king;
    long long black_pieces = black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;
    int value = 0;
    
    for (int i = 0; i < 64; i++) {
        if ((white_pieces & 1LL << i) != 0) {
            if ((white_pawns & 1LL << i) != 0) {
                value += 100;
            } else if ((white_rooks & 1LL << i) != 0) {
                value += 500;
            } else if ((white_knights & 1LL << i) != 0) {
                value += 300;
            } else if ((white_bishops & 1LL << i) != 0) {
                value += 300;
            } else if ((white_queens & 1LL << i) != 0) {
                value += 900;
            } else if ((white_king & 1LL << i) != 0) {
                value += 100000;
            } 
        } else if  ((black_pieces & 1LL << i) != 0) {
            if ((black_pawns & 1LL << i) != 0) {
                value -= 100;
            } else if ((black_rooks & 1LL << i) != 0) {
                value -= 500;
            } else if ((black_knights & 1LL << i) != 0) {
                value -= 300;
            } else if ((black_bishops & 1LL << i) != 0) {
                value -= 300;
            } else if ((black_queens & 1LL << i) != 0) {
                value -= 900;
            } else if ((black_king & 1LL << i) != 0) {
                value -= 100000;
            }
        }
    }

    return value;
}

int Board::countMaterial() {
    

    long long white_pieces = white_pawns | white_rooks | white_knights | white_bishops | white_queens | white_king;
    long long black_pieces = black_pawns | black_rooks | black_knights | black_bishops | black_queens | black_king;
    int value = 0;
    
    for (int i = 0; i < 64; i++) {
        if ((white_pieces & 1LL << i) != 0) {
            if ((white_pawns & 1LL << i) != 0) {
                value += 1;
            } else if ((white_rooks & 1LL << i) != 0) {
                value += 5;
            } else if ((white_knights & 1LL << i) != 0) {
                value += 3;
            } else if ((white_bishops & 1LL << i) != 0) {
                value += 3;
            } else if ((white_queens & 1LL << i) != 0) {
                value += 9;
            }
        } else if  ((black_pieces & 1LL << i) != 0) {
            if ((black_pawns & 1LL << i) != 0) {
                value += 1;
            } else if ((black_rooks & 1LL << i) != 0) {
                value += 5;
            } else if ((black_knights & 1LL << i) != 0) {
                value += 3;
            } else if ((black_bishops & 1LL << i) != 0) {
                value += 3;
            } else if ((black_queens & 1LL << i) != 0) {
                value += 9;
            }
        }
    }

    return value;
}
