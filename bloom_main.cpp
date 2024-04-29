#include<iostream>
#include<vector>
#include<string>

#include "bloom_engine.h"
#include "bloom_board.h"
#include "bloom_util.h"
#include "bloom_tests.h"



using namespace std;


int main() {


    string fen_string = "rn1q1rk1/p1b2ppp/2p2n2/4p3/2B3bN/2NP4/PPPQ1PPP/R1B2RK1 w - - 8 11";
    
    
    Board new_board(fen_string);

    new_board.printBoard();

    new_board.transposition_table.resize(4000000);

    new_board.transposition_table.clear();
    new_board.transposition_table.check();

    printQuickFindBestMove(new_board, 6, 1, 5);

    new_board.transposition_table.check();


    
}
