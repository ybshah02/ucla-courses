// Player.cpp

#include "provided.h"
#include "support.h"
#include "iostream"
#include <string>
#include <vector>
using namespace std;

class HumanPlayerImpl
{
  public:
    int chooseMove(const Scaffold& s, int N, int color);
};

class BadPlayerImpl
{
  public:
    int chooseMove(const Scaffold& s, int N, int color);
};

class SmartPlayerImpl
{
  public:
    int chooseMove(const Scaffold& s, int N, int color);
    int rate(Scaffold& gameboard, int N, int lastCol, int color, int depth);
    BestMove determineBestMove(AlarmClock &ac, Scaffold& gameboard, int N, int color, int spColor, int depth);
};

int HumanPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    if (s.numberEmpty() == 0)
        return -1;
    
    int col = -1;
    //user input for column
    cout << "Select a Column: ";
    cin >> col;
    
    Scaffold other = s;
    //repeatedly ask user until column selected is within bounds and column isn't full already
    while (!other.makeMove(col, color)){
        cout << "Invalid Column. Try again." << endl;
        cout << "Select a Column: ";
        cin >> col;
    }
    
    return col;
}

int BadPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    if (s.numberEmpty() == 0)
        return -1;
    
    Scaffold other = s;
    for (int i = 1; i <= s.cols(); i++){
        //validate if move is valid
        if(other.makeMove(i,color)){
            return i;
        }
    }
    
    return -1;
}

int SmartPlayerImpl::chooseMove(const Scaffold& s, int N, int color)
{
    //if scaffold is full, no move
    if(s.numberEmpty() == 0)
        return -1;
    
    Scaffold other = s;
    AlarmClock ac(9900);
    //choose the best move using minimax algorithm
    return determineBestMove(ac, other, N, color, color, 0).getBestCol();
}

int SmartPlayerImpl::rate(Scaffold& gameboard, int N, int lastCol, int color, int depth){
    
    int winner1 = -2;
    int winner2 = -2;
    int turnCount1 = 0;
    int turnCount2 = 0;
    int otherColor = -1;
    
    //determine the color and turncount for determineWinner function
    if (color == RED){
        turnCount1 = 2;
        otherColor = BLACK;
        turnCount2 = 1;
    }else if (color == BLACK){
        turnCount1 = 1;
        turnCount2 = 2;
        otherColor = RED;
    }
    //determine if winner 1 or 2 has won
    determineWinner(gameboard, N, lastCol, winner1, turnCount1);
    determineWinner(gameboard, N, lastCol, winner2, turnCount2);
    
    //if winner 1 or 2 has won, set completed to true and return the rating of the move
    if (winner1 == RED || winner2 == RED || winner1 == BLACK || winner2 == BLACK || winner1 == TIE_GAME || winner2 == TIE_GAME){
        if (winner1 == color)
            return (9999-depth);
        else if (winner2 == otherColor)
            return -(9999-depth);
        else if (winner1 == TIE_GAME && winner2 == TIE_GAME)
            return 0;
    }
    return -10000;
}

BestMove SmartPlayerImpl::determineBestMove(AlarmClock &ac, Scaffold& gameboard, int N, int color, int spColor, int depth){
    
    //stores all the columns and ratings of moves using BestMoves object
    vector<BestMove> moveResults;

    //parse through each column
    for (int i = 1; i <= gameboard.cols(); i++){
        //check if the alarm clock has timed out
        if (!ac.timedOut()){
            //check if column can make the move
            if (gameboard.makeMove(i, color)){

                //store rating
                int rating = rate(gameboard, N, i, spColor, depth);

                //if game completed, store the move
                if (rating >= -9999 & rating <= 9999){
                    moveResults.push_back(BestMove(i,rating));
                }
                //if not, continue searching for moves and store once found completed
                else if (depth + 1 < MAX_DEPTH){
                    //alternate color for recursive move to check other player's rating
                    if (color == RED)
                        color = BLACK;
                    else if (color == BLACK)
                        color = RED;
                    
                    moveResults.push_back(BestMove(i, determineBestMove(ac, gameboard,N,color,spColor,depth+1).getRating()));
                }
                //if no best move found to complete game, force tie
                else{
                    moveResults.push_back(BestMove(i,0));
                }
                //undo the moves in the scaffold
                gameboard.undoMove();
            }
        }
    }
    
    int index = 0;
    //if the color is the computer color, want to find the max rating
    if (color == spColor){
        for (int i = 0; i < moveResults.size(); i++){
            if (moveResults[i].getRating() > moveResults[index].getRating()){
                index = i;
            }
        }
    //if not, want to find the min rating
    }else{
        for (int i = 0; i < moveResults.size(); i++){
            if (moveResults[i].getRating() < moveResults[index].getRating()){
                index = i;
            }
        }
    }
    
    //return the bestmove
    return moveResults[index];
}

//******************** Player derived class functions *************************

//  These functions simply delegate to the Impl classes' functions.
//  You probably don't want to change any of this code.

HumanPlayer::HumanPlayer(string nm)
 : Player(nm)
{
    m_impl = new HumanPlayerImpl;
}
 
HumanPlayer::~HumanPlayer()
{
    delete m_impl;
}
 
int HumanPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}

BadPlayer::BadPlayer(string nm)
 : Player(nm)
{
    m_impl = new BadPlayerImpl;
}
 
BadPlayer::~BadPlayer()
{
    delete m_impl;
}
 
int BadPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}

SmartPlayer::SmartPlayer(string nm)
 : Player(nm)
{
    m_impl = new SmartPlayerImpl;
}
 
SmartPlayer::~SmartPlayer()
{
    delete m_impl;
}
 
int SmartPlayer::chooseMove(const Scaffold& s, int N, int color)
{
    return m_impl->chooseMove(s, N, color);
}
