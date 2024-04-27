#include<vector>
#include<iostream>
#include<chrono>

#include "bloom_board.h"
#include "bloom_engine.h"


using namespace std;

int stabilizePosition(Board& board, int player, int alpha, int beta, int qdepth, chrono::time_point<chrono::steady_clock> end_point) {
    if (qdepth == 10) {
        //printBitBoard(board);
        //cout << "Stabilize position called at qdepth: " << qdepth << " player: " << player << " alpha: " << alpha << " beta: " << beta <<  "\n";

    }
    
    if (chrono::steady_clock::now() > end_point) {
        return 0;
    }

    long long castling_rights_copy = board.castling_rights;
    int en_passant_i_copy = board.en_passant_i;
    unsigned long long hash_copy = board.zobrist_hash;

    if (!board.white_king) {
        return -100000;
    } else if (!board.black_king) {
        return 100000;
    }

    int evaluation = board.evaluate();

    
    if (player == 1) {
        if (evaluation > alpha) {
            alpha = evaluation;
            if (evaluation >= beta) {
                return beta;
            }
        }
    } else {
        if (evaluation < beta) {
            beta = evaluation;
            if (evaluation < alpha) {
                return alpha;
            }
        }
    }
    


    vector<LegalMove> legal_takes = board.generateLegalTakes(player);
    
    vector<int> values;

    

    if (legal_takes.size() == 0 || qdepth == 0) {
        return evaluation;
    } else {
        int max = evaluation;
        int min = evaluation;


        for (LegalMove move:legal_takes) {



            long long& replace_type = board.getPieceType(move.to_i);
            long long& type = board.getPieceType(move.from_i);
            int type_index = board.getPieceTypeIndex(move.from_i);
            int replace_type_index = board.getPieceTypeIndex(move.to_i);
            
            bool take = (replace_type != 0);

            board.makeMove(move, type, replace_type, type_index, replace_type_index);

            int result_evaluation = stabilizePosition(board, player * -1, alpha, beta, qdepth -1, end_point);

            board.reverseMove(move, type, replace_type, castling_rights_copy, en_passant_i_copy, hash_copy,  take);
            
            if (result_evaluation > max) {
                max = result_evaluation;
                
            }
            if (result_evaluation < min) {
                min = result_evaluation;
            }

            

            if (player == 1) {
                if (result_evaluation > alpha) {
                    alpha = result_evaluation;
                    if (beta <= alpha) {
                        //branches_pruned += 1;
                        break;
                    }
                }
            } else {
                if (result_evaluation < beta) {
                    beta = result_evaluation;
                    if (beta <= alpha) {
                        //branches_pruned += 1;
                        break;
                    }
                }
            }
            

        }

        if (player == 1) {
            return max;
        } else {
            return min;
        }
    }
}

Move findBestMove(Board& board, int depth, int player, LegalMove previous_move, int alpha, int beta, int quiescence_depth, chrono::time_point<chrono::steady_clock> end_point) {

    if (chrono::steady_clock::now() > end_point) {
        return {0,0, player, 0 , {}};
    }
    
    
    TranspositionEntry retrieved_entry = board.transposition_table.retrieve(board.zobrist_hash);
    LegalMove table_move = {0, 0};
    if (retrieved_entry.key != 0) {
        //transpositions_found += 1;
        if (retrieved_entry.depth >= depth) {
            table_move = retrieved_entry.best_move;
            //return {0,0, player, retrieved_entry.valuation , {retrieved_entry.best_move}};
        } else {
            table_move = retrieved_entry.best_move;
        }
    }
    
    
    //moves_searched += 1;
    



    long long castling_rights_copy = board.castling_rights;
    int en_passant_i_copy = board.en_passant_i;
    unsigned long long hash_copy = board.zobrist_hash;
    

    //printBitBoard(board);
    
    
    //cout << "Player : " << player << " Depth: " << depth << "\n";
    //cout << "Board Player: " << board.white_move << "\n";
    //cout << "En passant: " << board.en_passant_i << "\n";

    /*
    if (board.zobrist_hash != board.generateZobristHash(zobrist_key_storage)) {
        throw domain_error("Zobrist Hashes do not match");
    }
    */

    if (!board.white_king) {
        Move new_move = {0,0,player, -100000 - depth, {}};
        // cout << "Winning Move for black at depth: " << depth << " with Move: " << previous_move.from_i << "," << previous_move.to_i << "\n";
        return new_move;
        //return -100000;
    } else if (!board.black_king) {
        Move new_move = {0,0,player, 100000 + depth, {}};
        // cout << "Winning Move for white at depth: " << depth << " with Move: " << previous_move.from_i << "," << previous_move.to_i << "\n";
        return new_move;
        //return 100000;
    } else if (depth == 0) {

        if (quiescence_depth > 0) {
            
            Move new_move = {0,0,player, stabilizePosition(board, player, alpha, beta, quiescence_depth), {}}; 
            return new_move;
        } else {
            Move new_move = {0,0,player, board.evaluate(), {}}; 
            return new_move;
        }
        
    }

    vector<LegalMove> legal_moves = board.generateLegalMoves(player, (depth <= 0), table_move);
    
    vector<int> values;

    if (legal_moves.size() == 0) {
        Move new_move;
        
        if (board.checkForChecks() == player) {
            if (player == -1) {
                new_move = {0,0,player, (100000 + depth), {}};
                return new_move;
            } else {
                new_move = {0,0,player, (-100000 - depth), {}};
                return new_move;
            }
            
        }
        else {
            new_move = {0,0,player, 0, {}};
        }
        
        // cout << "Draw at: " << depth << " with Move: " << previous_move.from_i << "," << previous_move.to_i << "\n";
        return new_move;
        //return 0;
    }


    int max = -1000000;
    Move max_move;
    LegalMove max_legal_move;
    int min = 1000000;
    Move min_move;
    LegalMove min_legal_move;


    for (LegalMove move:legal_moves) {

        bool take = false;

        long long& replace_type = board.getPieceType(move.to_i);
        long long& type = board.getPieceType(move.from_i);
        int type_index = board.getPieceTypeIndex(move.from_i);
        int replace_type_index = board.getPieceTypeIndex(move.to_i);
        
        take = (replace_type != 0);

        

        board.makeMove(move, type, replace_type, type_index, replace_type_index);

        Move result_move = findBestMove(board, depth - 1, player * -1, move, alpha, beta, quiescence_depth, end_point);

        board.reverseMove(move, type, replace_type, castling_rights_copy, en_passant_i_copy, hash_copy,  take);
        
        if (result_move.valuation > max) {
            max = result_move.valuation;
            max_move = result_move;
            max_legal_move = move;
        }
        if (result_move.valuation < min) {
            min = result_move.valuation;
            min_move = result_move;
            min_legal_move = move;
        }

        if (player == 1) {
            if (result_move.valuation > alpha) {
                alpha = result_move.valuation;
                if (beta <= alpha) {
                    //branches_pruned += 1;
                    break;
                }
            }
        } else {
            if (result_move.valuation < beta) {
                beta = result_move.valuation;
                if (beta <= alpha) {
                    //branches_pruned += 1;
                    break;
                }
            }
        }


    }
    // cout << "\n" ;

    if (player == 1) {
        
        max_move.move_stack.push_back(max_legal_move);
        // cout << "Best Move Found (for white): " << max_legal_move.from_i << "," << max_legal_move.to_i << " With best max value: "<< max << " At depth: " << depth << " from Move: " << previous_move.from_i << "," << previous_move.to_i << "\n";
        board.transposition_table.store(board.zobrist_hash, depth, max_legal_move, max_move.valuation);
        return max_move;
    } else {
        
        min_move.move_stack.push_back(min_legal_move);
                // cout << "Best Move Found (for black): " << min_legal_move.from_i << "," << min_legal_move.to_i << " With best min value: "<< min << " At depth: " << depth <<  " from Move: " << previous_move.from_i << "," << previous_move.to_i <<"\n";
        board.transposition_table.store(board.zobrist_hash, depth, min_legal_move, min_move.valuation);
        return min_move;
        //return min;
    }
}

Move iterativeDeepening(Board& board, int seconds, int player, int qdepth) {
    auto start = chrono::steady_clock::now();

    chrono::milliseconds time_limit(seconds * 1000);

    int quiescence_depth = 0; 


    auto end_point = start + time_limit;

    board.transposition_table.clear();
    int depth = 1;
    LegalMove previous_move = {0, 0};
    Move best_move;
    Move found_move;

    vector<int> durations;

    
    while(chrono::steady_clock::now() - start <= time_limit) {
        found_move = findBestMove(board, depth, player, previous_move, -1000000, 1000000, qdepth, end_point);

        if (chrono::steady_clock::now() < end_point) {
            best_move = found_move;
            auto duration = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count();
            durations.push_back(duration);
            //cout << "Depth: " << depth << " reached in " << duration << " microseconds!" << "\n";

            vector<double> duration_factors;
            for(int i = 1; i < durations.size(); i++) {
                double factor = durations[i] / double(durations[i-1]);
                duration_factors.push_back(factor);
            }
            double factor_sum = accumulate(duration_factors.begin(), duration_factors.end(), 0.0);
            double factor_mean = factor_sum / duration_factors.size();

            depth += 1;

            if (duration * factor_mean / 2 > time_limit.count() * 1000) {
                //break;
                continue;
            }


            
        }
        board.transposition_table.clear();
        //board.transposition_table.check();
        

        
        
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Found Move: " << i_to_string(best_move.move_stack[best_move.move_stack.size() - 1].from_i) << " " << i_to_string(best_move.move_stack[best_move.move_stack.size() - 1].to_i) << " at depth: " << depth -1 << " in " << duration / 1000.0 << " seconds \n";
    //cout << "Branches pruned: " << branches_pruned << "\n"; 
    //cout << "Nodes Searched: " << moves_searched << "\n";
    //cout << "Quiescence Nodes Searches: " << quiescence_moves_searched << "\n";

    //cout << "Factors: ";

    /*
    for(int i = 1; i < durations.size(); i++) {
        double factor = durations[i] / double(durations[i-1]);
        cout << " " << factor << ", ";
    }
    */
    //cout << "\n";
    

    return best_move;

}

Move printIterativeDeepening(Board& board, int seconds, int player, int qdepth) {
    auto start = chrono::steady_clock::now();

    chrono::milliseconds time_limit(seconds * 1000);

    int quiescence_depth = 0; 


    auto end_point = start + time_limit;

    board.transposition_table.clear();
    int depth = 1;
    LegalMove previous_move = {0, 0};
    Move best_move;
    Move found_move;

    vector<int> durations;

    
    

    while(chrono::steady_clock::now() - start <= time_limit) {
        found_move = findBestMove(board, depth, player, previous_move, -1000000, 1000000, qdepth, end_point);

        if (chrono::steady_clock::now() < end_point) {
            best_move = found_move;
            auto duration = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - start).count();
            durations.push_back(duration);
            cout << "Depth: " << depth << " reached in " << duration << " microseconds!" << "\n";

            vector<double> duration_factors;
            for(int i = 1; i < durations.size(); i++) {
                double factor = durations[i] / double(durations[i-1]);
                duration_factors.push_back(factor);
            }
            double factor_sum = accumulate(duration_factors.begin(), duration_factors.end(), 0.0);
            double factor_mean = factor_sum / duration_factors.size();

            depth += 1;

            if (duration * factor_mean / 2 > time_limit.count() * 1000) {
                break;
                continue;
            }


            
        }
        board.transposition_table.clear();
        //board.transposition_table.check();
        

        
        
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Found Move: " << i_to_string(best_move.move_stack[best_move.move_stack.size() - 1].from_i) << " " << i_to_string(best_move.move_stack[best_move.move_stack.size() - 1].to_i) << " at depth: " << depth -1 << " in " << duration / 1000.0 << " seconds \n";
    //cout << "Branches pruned: " << branches_pruned << "\n"; 
    //cout << "Nodes Searched: " << moves_searched << "\n";
    //cout << "Quiescence Nodes Searches: " << quiescence_moves_searched << "\n";

    cout << "Factors: ";

    for(int i = 1; i < durations.size(); i++) {
        double factor = durations[i] / double(durations[i-1]);
        cout << " " << factor << ", ";
    }

    cout << "\n";
    

    return best_move;

}