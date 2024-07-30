//
//  Player.h
//  BeatThat
//
//

#ifndef Player_h
#define Player_h
#include "Die.h"
#include <string>

namespace cs31
{

    // this class manages two dies and tracks how many rounds have been won by this player
    // CS 31 Students have three TODO methods to complete here
    class Player
    {
    public:
        Player();
        
        void roll( );
        void roll( Die d1, Die d2 );  // cheating...
        // trivial getters
        Die  getDie1( ) const;
        Die  getDie2( ) const;
        
        std::string whatWasRolled();  // for testing...
        
        void wonARound( );
        int  getRoundsWon( ) const;
        Die  largestDie( ) const;
        Die  smallestDie( ) const;

    private:
        // Each Player has two six-sided dies
        Die  mDie1, mDie2;
        // Each Player tracks how many rounds it has won
        int  mRoundsWon;

    };
    
}

#endif /* Player_h */
