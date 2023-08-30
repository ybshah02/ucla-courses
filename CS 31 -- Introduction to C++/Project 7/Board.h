//
//  Board.h
//  BeatThat
//

#ifndef Board_h
#define Board_h
#include "Die.h"
#include <string>

namespace cs31
{
    
    // this class provides display( ) which is called with each round of play
    // to show the state of the game
    // CS 31 Students have six TODO methods to complete here
    class Board
    {
    public:
        Board();

        // trivial setters
        void setHumanRoundsWon( int rounds );
        void setComputerRoundsWon( int rounds );
        void setHumanDiceRolled( Die d1, Die d2 );
        void setComputerDiceRolled( Die d1, Die d2 );
        void setTurnsLeft( int turns );
        
        // trivial getters
        int getHumanRoundsWon( );
        int getComputerRoundsWon( );
        int getTurnsLeft( );
        
        void markComputerAsWinner();
        void markHumanAsWinner();
        void markTied();
        
        bool didHumanWin() const;
        bool didComputerWin() const;
        bool isGameOver( ) const;
        
        // major method called with each round of play by the BeatThat class
        std::string display( std::string message ) const;
        std::string endingMessage( ) const;
    private:
        // data members to track the game and winner
        bool mGameOver;
        bool mHumanWon;
        bool mComputerWon;
        // a count of how many rounds the Human has won
        int  mHumanRoundsWon;
        // a count of how many rounds the Computer has won
        int  mComputerRoundsWon;
        // what has the Human rolled?
        Die  mHumanDie1, mHumanDie2;
        // what has the Computer rolled?
        Die  mComputerDie1, mComputerDie2;
        // how many turns remain in the game?
        int  mTurnsLeft;
    };
}

#endif /* Board_h */
