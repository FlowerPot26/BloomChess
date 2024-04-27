#include<vector>
#include<iostream>

#include "bloom_board.h"
#include "bloom_engine.h"
#include "bloom_tests.h"


using namespace std;


void makePlayerMove(Board& board, int player, bool reverse) {
    string from_string; string to_string;
    
    cout << "Enter your next move indecies: ";
    cin >> from_string >> to_string;
    cout << "\n";
    int from_i = string_to_i(from_string); int to_i = string_to_i(to_string);



    if (from_i < 0 || to_i < 0 || from_i >= 64 || to_i >= 64 || from_i == to_i) {
        cout << "Indecies entered are invalid! \n";
        makePlayerMove(board, player, reverse);
        return;
    } else {
        vector<LegalMove> legal_moves = board.generateLegalMoves(player, false);
        bool legal = false;
        for (LegalMove legal_move:legal_moves) {
            if (legal_move.from_i == from_i && legal_move.to_i == to_i) {
                legal = true;
                break;
            }
        }

        if (legal) {
            long long& replace_type = board.getPieceType(to_i);
            long long& type = board.getPieceType(from_i);
            int replace_index = board.getPieceTypeIndex(to_i);
            int type_index = board.getPieceTypeIndex(from_i);
            long long mask = 0; 

            long long castling_rights_copy = board.castling_rights;
            int en_passant_i_copy = board.en_passant_i;
            unsigned long long hash_copy = board.zobrist_hash;

            /*
            if (type == board.white_rooks && from_i == 63) {
                mask = (1LL << 0);
            } else if (type == board.white_rooks && from_i == 56) {
                mask = (1LL << 1);
            } else if (type == board.black_rooks && from_i == 7) {
                mask = (1LL << 2);
            } else if (type == board.white_rooks && from_i == 0) {
                mask = (1LL << 3);
            } else if (type == board.white_king) {
                mask = (3LL);
            } else if (type == board.black_king) {
                mask = (12LL);
            }
            */

            LegalMove move = {from_i, to_i};
            board.makeMove(move, type, replace_type, type_index, replace_index);

            

            board.printBoard(from_i);
            if (reverse) {
                board.reverseMove(move, type, replace_type, castling_rights_copy, en_passant_i_copy, hash_copy);
            }
            return;
        } else {
            cout << "Move entered was invalid! \n";
            makePlayerMove(board, player, reverse);
            return;
        }

        
    }
}



void makeBotMove(Board& board, int player, int depth, int quiescence_depth) {
    LegalMove empty_move = {0,0};
    Move move = findBestMove(board, depth, player, empty_move, -1000000, 1000000, quiescence_depth);
    LegalMove found_move = move.move_stack[move.move_stack.size() - 1];
    int from_i = found_move.from_i; int to_i = found_move.to_i;

    long long& replace_type = board.getPieceType(found_move.to_i);
    long long& type = board.getPieceType(found_move.from_i);
    int replace_index = board.getPieceTypeIndex(found_move.to_i);
    int type_index = board.getPieceTypeIndex(found_move.from_i);

    board.makeMove(found_move, type, replace_type, type_index, replace_index);

    board.printBoard(from_i);
    return;
}

void makeTimedBotMove(Board& board, int player, int seconds, int qdepth) {
    LegalMove empty_move = {0,0};
    Move move = iterativeDeepening(board, seconds, player, qdepth);
    LegalMove found_move = move.move_stack[move.move_stack.size() - 1];
    int from_i = found_move.from_i; int to_i = found_move.to_i;

    long long& replace_type = board.getPieceType(found_move.to_i);
    long long& type = board.getPieceType(found_move.from_i);
    int replace_index = board.getPieceTypeIndex(found_move.to_i);
    int type_index = board.getPieceTypeIndex(found_move.from_i);

    board.makeMove(found_move, type, replace_type, type_index, replace_index);

    board.printBoard(from_i);
    return;
}


void printLegalMoves(Board& board, int player, bool only_takes,  LegalMove table_move) {
    vector<LegalMove> legal_moves;
    if (!(table_move.from_i == 0 && table_move.to_i == 0)) {
        legal_moves = board.generateLegalMoves(player, only_takes, table_move);
    } else {
        legal_moves = board.generateLegalMoves(player, only_takes);
    }
    
    cout << "Legal Moves for: " << player << "\n";
    for (LegalMove legal_move:legal_moves) {
        cout << "Legal Move: " << i_to_string(legal_move.from_i) << "," << i_to_string(legal_move.to_i) << ", Take:" << legal_move.take << " Priority: "<< legal_move.priority << "\n";
    }
    return;
}

void printLegalTakes(Board& board, int player, LegalMove table_move) {
    vector<LegalMove> legal_moves;
    legal_moves = board.generateLegalTakes(player);
    
    cout << "Legal Takes for: " << player << "\n";
    for (LegalMove legal_move:legal_moves) {
        cout << "Legal Take: " << i_to_string(legal_move.from_i) << "," << i_to_string(legal_move.to_i) << ", Take:" << legal_move.take << " Priority: "<< legal_move.priority << "\n";
    }
    return;
}

void simulateGame(int player_side, Board& board, int depth, int quiescence_depth) {
    bool end = false;
    bool white_move = board.white_move;
    while (!end) {
        if (white_move) {
            if (player_side == 1) {
                makePlayerMove(board, 1);
            } else {
                makeBotMove(board, 1, depth, quiescence_depth);
            }
        } else {
            if (player_side == 1) {
                makeBotMove(board, -1, depth, quiescence_depth);
            } else {
                makePlayerMove(board, -1);
            }
        }

        int winning_player = board.checkGameEnd();
        if (winning_player) {
            end = true;
            cout << "Player: " << winning_player << " has won the game!";
        }

        white_move = (!white_move);
    }
    return;
}

void simulateTimedGame(int player_side, Board& board, int seconds) {
    bool end = false;
    bool white_move = board.white_move;
    while (!end) {
        if (white_move) {
            if (player_side == 1) {
                makePlayerMove(board, 1);
            } else {
                makeTimedBotMove(board, 1, seconds);
            }
        } else {
            if (player_side == 1) {
                makeTimedBotMove(board, -1, seconds);
            } else {
                makePlayerMove(board, -1);
            }
        }

        int winning_player = board.checkGameEnd();
        if (winning_player) {
            end = true;
            cout << "Player: " << winning_player << " has won the game!";
        }

        white_move = (!white_move);
    }
    return;
}


void simulateBotGame(Board& board, int depth1, int quiescence_depth1, int depth2, int quiescence_depth2) {
    bool end = false;
    bool white_move = board.white_move;
    cout << "White Bot: Depth: " << depth1 << " QDepth: " << quiescence_depth1 << "\n";
    cout << "Black Bot: Depth: " << depth2 << " QDepth: " << quiescence_depth2 << "\n";
    while (!end) {
        if (white_move) {
            makeBotMove(board, 1, depth1, quiescence_depth1); 
        } else {
            makeBotMove(board, -1, depth2, quiescence_depth2); 
        }

        int winning_player = board.checkGameEnd();
        if (winning_player) {
            end = true;
            cout << "Player: " << winning_player << " has won the game!";
        }

        white_move = (!white_move);
    }
    return;
}

void simulateTimedBotGame(Board& board, int time1, bool quiescence1, int time2, bool quiescence2) {
    bool end = false;
    bool white_move = board.white_move;
    cout << "White Bot: Time: " << time1 << "\n";
    cout << "Black Bot: Time: " << time2 << "\n";
    while (!end) {
        board.transposition_table.clear();
        if (white_move) {
            makeTimedBotMove(board, 1, time1);
        } else {
            makeTimedBotMove(board, -1, time2);
        }

        int winning_player = board.checkGameEnd();
        if (winning_player) {
            end = true;
            cout << "Player: " << winning_player << " has won the game!";
        }

        white_move = (!white_move);
    }
    return;
}

void measureEvalSpeed(Board& board) {
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < 1000; i++) {
        int x = board.evaluate();
    }
    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    cout << "Execution duration: " << duration << " nanoseconds; " << duration / 1000000000.0 << "seconds;" << "\n";
    cout << "Evaluation: " << board.evaluate() << "\n";
}

void printFindBestMove(Board& board, int depth, int player, int quiescence_depth) {

    auto start = chrono::steady_clock::now();

    //moves_searched = 0;
    //quiescence_moves_searched = 0;

    LegalMove empty_move = {0,0};
    Move found_move = findBestMove(board, depth, player, empty_move, -1000000, 1000000, quiescence_depth);
    cout << "Move chain: " << "\n";
    for (int i = found_move.move_stack.size() - 1; i >= 0; i--) {
        cout << i_to_string(found_move.move_stack[i].from_i) << "," << i_to_string(found_move.move_stack[i].to_i) << "\n";
    }

    cout << "Final Move: " << i_to_string(found_move.move_stack[found_move.move_stack.size() - 1].from_i) << "," << i_to_string(found_move.move_stack[found_move.move_stack.size() - 1].to_i) << "\n";

    cout << "Valuation: " << found_move.valuation << "\n";  

    //cout << "Moves searched: " << moves_searched << "\n";

    //cout << "Transpositions found: " << transpositions_found << "\n";

    //cout << "Qiescence Moves searched: " << quiescence_moves_searched << "\n";

    

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    cout << "Execution duration: " << duration << " nanoseconds; " << duration / 1000000000.0 << "seconds;" << "\n";

    return;
}

void testAttackSquares(Board& board, int player) {
    cout << "Attack Squares of: " << player << "\n";
    for (int i = 0; i < 64; i++) {
        if (board.squareUnderAttack(i, player)) {
            cout << i_to_string(i)<< "\n";
        }
    }
    cout << "\n";
    return; 
}

void testChecks(Board& board) {
    cout << "Player in check: " << board.checkForChecks() << "\n";
}

void printBoardSetup(Board& board) {
    
    cout << "White Kingside Castle: " << ((board.castling_rights & (1LL << 0)) != 0) << "\n";
    cout << "White Queenside Castle: " << ((board.castling_rights & (1LL << 1)) != 0) << "\n";
    cout << "Black Kingside Castle: " << ((board.castling_rights & (1LL << 2)) != 0) << "\n";
    cout << "Black Queenside Castle: " << ((board.castling_rights & (1LL << 3)) != 0) << "\n";
    cout << "En passant: " << board.en_passant_i << "\n";
    cout << "White Move: " << board.white_move << "\n";
}

void findZobristDifference(ZobristKeyStorage& zobrist_key_storage, unsigned long long a, unsigned long long c) {
    if (a == c) {
        cout << "Zobrist Hashes are the same \n";
        return;
    }
    unsigned long long b = a ^ c;
    for (int i = 0; i < zobrist_key_storage.piece_squares.size(); i++) {
        if (b == zobrist_key_storage.piece_squares[i]) {
            cout << "Zobrist Key Matches: Piece Squares " << i << "\n";
            return;
        }
    }
    for (int i = 0; i < zobrist_key_storage.en_passant_squares.size(); i++) {
        if (b == zobrist_key_storage.en_passant_squares[i]) {
            cout << "Zobrist Key Matches: En Passant  " << i << "\n";
            return;
        }
    }
    for (int i = 0; i < zobrist_key_storage.castling_rights.size(); i++) {
        if (b == zobrist_key_storage.castling_rights[i]) {
            cout << "Zobrist Key Matches: Castling Rights  " << i << "\n";
            return;
        }
    }
    if (b == zobrist_key_storage.white_turn) {
        cout << "Zobrist Key Matches: White Move  \n";
        return;
    }
    cout << "No easy zobrist difference found :/ \n";
    
}

void checkReplacements(Board& board) {
    cout << "Replacements: " << board.transposition_table.replacements << "\n";
}



