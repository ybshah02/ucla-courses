//
//  BeatThat.cpp
//  BeatThat
//

#include "BeatThat.h"
#include <iostream>
using namespace std;

namespace cs31
{
    // human goes first
    BeatThat::BeatThat() : mHuman(), mComputer(), mBoard(), mTurnCount(0), mBoardMessage( "" )
    {
        
    }

    // called to allow the human player to randomly roll its dice
    void BeatThat::humanPlay( )
    {
        mHuman.roll();
    }
    
    // called to force a certain human player roll
    void BeatThat::humanPlay( Die d1, Die d2 )
    {
        mHuman.roll(d1, d2);
    }

    // called to allow the computer player to randomly roll its dice
    void BeatThat::computerPlay( )
    {
        mComputer.roll();
    }

    // called to force a certain computer player roll
    void BeatThat::computerPlay( Die d1, Die d2 )
    {
        mComputer.roll(d1, d2);
    }
        
    // called by isGameOver( ) to figure out the current state of the game
    // should properly return either: GAMEOUTCOME::GAMENOTOVER,
    //                                GAMEOUTCOME::HUMANWONGAME,
    //                                GAMEOUTCOME::COMPUTERWONGAME or
    //                                GAMEOUTCOME::TIEDGAME
    BeatThat::GAMEOUTCOME  BeatThat::determineGameOutcome( ) const
    {
        GAMEOUTCOME result;
        if (mTurnCount < 5){
            result = GAMEOUTCOME::GAMENOTOVER;
        }else{
            if (mHuman.getRoundsWon() > mComputer.getRoundsWon()){
                result = GAMEOUTCOME::HUMANWONGAME;
            }else if(mComputer.getRoundsWon() > mHuman.getRoundsWon()){
                result = GAMEOUTCOME::COMPUTERWONGAME;
            }else{
                result = GAMEOUTCOME::TIEDGAME;
            }
        }
        
        return result;
    }
    
    bool BeatThat::isGameOver()
    {
        GAMEOUTCOME result = determineGameOutcome();
        if (result != GAMEOUTCOME::GAMENOTOVER)
        {
            if (result == GAMEOUTCOME::HUMANWONGAME)
            {
                mBoard.markHumanAsWinner();
            }
            else if (result == GAMEOUTCOME::COMPUTERWONGAME)
            {
                mBoard.markComputerAsWinner();
            }
            else
            {
                mBoard.markTied();
            }
        }
        return( result != GAMEOUTCOME::GAMENOTOVER );
    }

    // trivial getters
    Player BeatThat::getHuman() const
    {
        return( mHuman );
    }
    
    Player BeatThat::getComputer() const
    {
        return( mComputer );
    }
    
    Board  BeatThat::getBoard() const
    {
        return( mBoard );
    }

    // game output messages including the board message
    std::string BeatThat::display( std::string msg )
    {
       using namespace std;
       std::string s( "" );
    
        s = mBoard.display( mBoardMessage );
        if (!isGameOver())
        {
            s = s + msg;
        }

        return( s );
    }

    std::string BeatThat::endingMessage() const
    {
        return( mBoard.endingMessage() );
    }

    // review the roll dice and determine who won this round or it ended tied
    void BeatThat::evaluateRolls( )
    {
        int humanLargestNumber = mHuman.largestDie( ).getValue();
        int computerLargestNumber = mComputer.largestDie().getValue();
 
        int humanSmallestNumber = mHuman.smallestDie( ).getValue();
        int computerSmallestNumber = mComputer.smallestDie().getValue();
        
        if (humanLargestNumber > computerLargestNumber)
        {
            // human won this turn...
            mHuman.wonARound();
            mBoardMessage = HUMAN_WON_TURN;
        }
        else if (computerLargestNumber > humanLargestNumber)
        {
            // computer won this turn...
            mComputer.wonARound();
            mBoardMessage = COMPUTER_WON_TURN;
        }
        else
        {
            // largest dies were equal so move onto the smallest dies
            if (humanSmallestNumber > computerSmallestNumber)
            {
                // humaan won this turn...
                mHuman.wonARound();
                mBoardMessage = HUMAN_WON_TURN;
            }
            else if (computerSmallestNumber > humanSmallestNumber)
            {
                // computer won this turn...
                mComputer.wonARound();
                mBoardMessage = COMPUTER_WON_TURN;
            }
            else
            {
                mBoardMessage = NO_ONE_WON_TURN;
            }
        }
    }

    int BeatThat::turnsLeft() const
    {
        return( MAX_NUMBER_OF_TURNS - mTurnCount );
    }

    // update the board for the latest round of play
    void BeatThat::updateBoard( )
    {
        mBoard.setComputerRoundsWon( mComputer.getRoundsWon() );
        mBoard.setHumanRoundsWon( mHuman.getRoundsWon() );
        mBoard.setHumanDiceRolled(mHuman.getDie1(), mHuman.getDie2());
        mBoard.setComputerDiceRolled(mComputer.getDie1(), mComputer.getDie2());
        mBoard.setTurnsLeft(turnsLeft());
    }

    // called to indicate a turn has ended
    // increment the turn counter
    // evaluate the rolled dice
    // update the board for the latest round of play
    void BeatThat::endTurn( )
    {
        mTurnCount += 1;
        evaluateRolls( );
        updateBoard( );
    }

}
