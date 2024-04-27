#include<iostream>
#include<vector>
#include<string>

#include "bloom_engine.h"
#include "bloom_board.h"
#include "bloom_util.h"
#include "bloom_tests.h"



using namespace std;


int main() {


    string fen_string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    
    Board new_board(fen_string);

    new_board.printBoard();

    
    printFindBestMove(new_board, 6, -1, 4);

    
}
