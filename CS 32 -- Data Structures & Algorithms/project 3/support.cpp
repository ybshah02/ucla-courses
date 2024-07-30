#include "provided.h"
#include "support.h"
#include <iostream>
using namespace std;

BestMove::BestMove(int col, int value):m_bestCol(col), m_rating(value){}

bool isSequence(Scaffold& gameboard, int N, int moveColor, int moveCol, int moveLevel) {
    
    //validate inputs
    if (moveColor == VACANT || moveCol < 1 || moveCol > gameboard.cols() || moveLevel < 1 || moveLevel > gameboard.levels()){
        return false;
    }
    
    //check horizontal
    
    //instantiate temp variables
    int sequenceCount = 0;
    int col = moveCol;
    int level = moveLevel;
    bool cont = true;
    
    //check to the left
    //iterate until told to stop
    while (cont){
        if (col > 0){
            //check if checker at column to the left is same as the color of the checker we are tracking
            if(gameboard.checkerAt(col, level) == moveColor){
                //iterate count of sequence
                sequenceCount++;
                //iterate to check next column
                col--;
            }else{
                //if checker isn't same color as checker we are tracking, then no need to continue and check next checkers
                cont = false;
            }
        }else{
            cont = false;
        }
    }
    
    //instantiate temp variables again
     col = moveCol+1;
     level = moveLevel;
     cont = true;
    
    //check to the right
    
    //iterate until told to stop
    while (cont){
        //check if checker at column to the right is same as the color of the checker we are tracking
        if (col <= gameboard.cols()){
            if(gameboard.checkerAt(col, level) == moveColor){
                sequenceCount++;
                col++;
            }else{
                cont = false;
            }
        }else{
            cont = false;
        }
    }
    
    //check if the sequence has at least N in a row
    if (sequenceCount >= N)
        return true;
    
    //check vertical
    
    //instantiate temp variables again
    sequenceCount = 0;
    col = moveCol;
    level = moveLevel;
    cont = true;
    
    while (cont){
        //check if checker at level below is same as the color of the checker we are tracking
        if (level > 0){
            if(gameboard.checkerAt(col, level) == moveColor){
                sequenceCount++;
                level--;
            }else{
                cont = false;
            }
        }else{
            cont = false;
        }
    }
    
    if (sequenceCount >= N)
        return true;
    
    //check diagonal (positive slope)
    
    sequenceCount = 0;
    col = moveCol;
    level = moveLevel;
    cont = true;
    
    //check upward and to the right
    
    while (cont){
        //check if checker at column to the right and one level up is same as the color of the checker we are tracking
        if (col <= gameboard.cols() && level <= gameboard.levels()){
            if(gameboard.checkerAt(col, level) == moveColor){
                sequenceCount++;
                level++;
                col++;
            }else{
                cont = false;
            }
        }else{
            cont = false;
        }
    }
    
    //check downward and to the left
    
    col = moveCol-1;
    level = moveLevel-1;
    cont = true;
    
    while (cont){
        //check if checker at column to the left and one level down is same as the color of the checker we are tracking
        if (col > 0 && level > 0){
            if(gameboard.checkerAt(col, level) == moveColor){
                sequenceCount++;
                level--;
                col--;
            }else{
                cont = false;
            }
        }else{
            cont = false;
        }
    }
    
    if (sequenceCount >= N)
        return true;
    
    //check diagonal (negative slope)
    
    sequenceCount = 0;
    col = moveCol;
    level = moveLevel;
    cont = true;
    
    //check upwards and to the left
    
    while (cont){
        //check if checker at column to the left and one level up is same as the color of the checker we are tracking
        if (col > 0 && level <= gameboard.levels()){
            if(gameboard.checkerAt(col, level) == moveColor){
                sequenceCount++;
                level++;
                col--;
            }else{
                cont = false;
            }
        }else{
            cont = false;
        }
    }
    
    //check diagonal downward and to the right
    
    col = moveCol+1;
    level = moveLevel-1;
    cont = true;
    
    while (cont){
        //check if checker at column to the right and one level down is same as the color of the checker we are tracking
        if (col <= gameboard.cols() && level > 0){
            if(gameboard.checkerAt(col, level) == moveColor){
                sequenceCount++;
                level--;
                col++;
            }else{
                //if checker isn't same color as checker we are tracking, then no need to continue and check next checkers
                cont = false;
            }
        }else{
            cont = false;
        }
    }
    
    if (sequenceCount >= N)
        return true;
    
    return false;
}

int determineWinner(Scaffold& gameboard, int N, int lastCol, int& winner, int turnCount){
    //if number in row is 0, then the game is at default completed but no winner
    if (N <= 0){
        winner = -3;
        return winner;
    }
    
    //if N in a row is not achievable on scaffold, there can be no winner so game is tied
    if (N > gameboard.cols() && N > gameboard.levels()){
        winner = -3;
        return winner;
    }
    
    int lastLevel = 0;
    
    //find level of the last move
    for (int i = 0; i < gameboard.levels(); i++){
        if (gameboard.checkerAt(lastCol, i) != VACANT){
            lastLevel++;
        }
    }
    
    int moveColor = VACANT;
    
    //find checker color of previous turn
    if (turnCount % 2 == 0){
        moveColor = RED;
    }else if (turnCount % 2 == 1){
        moveColor = BLACK;
    }
    
    //check if there is a sequence
    bool gameOver = isSequence(gameboard, N, moveColor, lastCol, lastLevel);
    
    //if no sequence, but the gameboard has no more empty slots, the game is tied
    if (!gameOver && gameboard.numberEmpty() == 0){
        winner = TIE_GAME;
        return winner;
    }
    
    //if game over, set the winner to the respecive color
    if (gameOver){
        winner = moveColor;
    }else{
        winner = -2;
    }
    
    return winner;
}

