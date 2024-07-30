//
//  BeatThat.h
//  BeatThat
//

#ifndef BeatThat_h
#define BeatThat_h

#include <string>
#include "Die.h"
#include "Player.h"
#include "Board.h"

namespace cs31
{

    // this class implements the rules of BeatThat game
    // CS 31 Students have five TODO methods to complete here
    class BeatThat
    {
    public:
        BeatThat();
        
        enum GAMEOUTCOME { HUMANWONGAME, COMPUTERWONGAME, TIEDGAME, GAMENOTOVER };
        // TODO: called by isGameOver( ) to figure out the current state of the game
        GAMEOUTCOME determineGameOutcome( ) const;
        
        // called to allow the human player to randomly roll
        void humanPlay( );
        // called to force a certain human player roll
        void humanPlay( Die d1, Die d2 );  // cheating...
        // called to allow the computer player to randomly roll
        void computerPlay( );
        // called to force a certain computer player roll
        void computerPlay( Die d1, Die d2 ); // cheating...

        
        // called to determine if play should be ended and to perform final board updates
        bool isGameOver();
        // called to determine how many more rounds of play remain
        int  turnsLeft( ) const;
        // called to indicate a turn has ended
        void endTurn( );
        
        // to test the state of the game...
        // trivial getters
        Player getHuman() const;
        Player getComputer() const;
        Board  getBoard() const;
        
        std::string display( std::string msg = "" );
        std::string endingMessage( ) const;
    private:
        // each BeatThat game has two Players and a Board
        Player mHuman;
        Player mComputer;
        Board  mBoard;
        // each BeatThat game tracks how many turns have taken place
        int    mTurnCount;
        // each BeatThat game can create a board message to be displayed as the game progresses
        std::string mBoardMessage;
        
        // review the roll dice and determine who won this round or it ended tied
        void evaluateRolls( );
        // update the board for the latest round of play
        void updateBoard( );

        // some important constants
        const int MAX_NUMBER_OF_TURNS       = 5;
        const std::string HUMAN_WON_TURN    = "---> human won this turn...";
        const std::string COMPUTER_WON_TURN = "---> computer won this turn...";
        const std::string NO_ONE_WON_TURN   = "---> no one won this turn...";
    };


}


#endif /* BeatThat_h */
