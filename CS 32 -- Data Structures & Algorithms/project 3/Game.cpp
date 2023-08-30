// Game.cpp

#include "provided.h"
#include "support.h"
#include <iostream>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nColumns, int nLevels, int N, Player* red, Player* black);
    bool completed(int& winner) const;
    bool takeTurn();
    void play();
    int checkerAt(int c, int r) const;
    bool isSequence(Scaffold& s, int N, int moveColor, int lastCol, int lastLevel) const;
private:
    Scaffold* m_gameboard;
    Player* m_red;
    Player* m_black;
    int m_winner;
    int m_turnCount;
    int m_N;
    int m_lastCol;
};

GameImpl::GameImpl(int nColumns, int nLevels, int N, Player* red, Player* black): m_red(red), m_black(black), m_turnCount(1), m_N(N), m_winner(TIE_GAME), m_lastCol(1)
{
    m_gameboard = new Scaffold(nLevels, nColumns);
}

bool GameImpl::completed(int& winner) const
{
    //determine if there is a winner
    determineWinner(*m_gameboard, m_N, m_lastCol, winner, m_turnCount);
    if (winner == RED || winner == BLACK || winner == TIE_GAME || winner == -3)
        return true;
    else
        return false;
}

bool GameImpl::takeTurn()
{
    if (completed(m_winner))
        return false;
    
    //red moves first - on odd turns
    if (m_turnCount % 2 != 0){
        //get col of red's chosen move
        int col = m_red->chooseMove(*m_gameboard, m_N, RED);
        //update grid with player 1's move
        m_gameboard->makeMove(col, RED);
        //track last col
        m_lastCol = col;
    //black moves second - on even turns
    }else{
        //get col of black's chosen move
        int col = m_black->chooseMove(*m_gameboard, m_N, BLACK);
        //update grid with player 2's move
        m_gameboard->makeMove(col, BLACK);
        //track last col
        m_lastCol = col;
    }
    //increment num of turns
    m_turnCount++;
    return true;
    
}

void GameImpl::play()
{
    int check = 0;
    while (!completed(m_winner)){
        //display board
        m_gameboard->display();
        //check if both players aren't interactive
        if (!m_red->isInteractive() & !m_black->isInteractive()){
            //on every fourth move, prompt viewer to press enter to continue
            if (check % 4 == 0){
                cout << "Press Enter to Continue..." << endl;
                cin.ignore(10000, '\n');
            }
            check++;
            //next turn
            takeTurn();
        }
        else{
            //next turn
            takeTurn();
        }
    }
    
    m_gameboard->display();
    
    if (m_winner == RED){
        cout << "Game over. " << m_red->name() << " has won the game." << endl;
    }else if (m_winner == BLACK){
        cout << "Game over. " << m_black->name() << " has won the game." << endl;
    }else if (m_winner == TIE_GAME){
        cout << "Game over. The game is tied." << endl;
    }
    
}

int GameImpl::checkerAt(int c, int r) const
{
    return m_gameboard->checkerAt(c, r);
}


//******************** Game functions *******************************

//  These functions simply delegate to GameImpl's functions.
//  You probably don't want to change any of this code.

Game::Game(int nColumns, int nLevels, int N, Player* red, Player* black)
{
    m_impl = new GameImpl(nColumns, nLevels, N, red, black);
}
 
Game::~Game()
{
    delete m_impl;
}
 
bool Game::completed(int& winner) const
{
    return m_impl->completed(winner);
}

bool Game::takeTurn()
{
    return m_impl->takeTurn();
}

void Game::play()
{
    m_impl->play();
}
 
int Game::checkerAt(int c, int r) const
{
    return m_impl->checkerAt(c, r);
}
