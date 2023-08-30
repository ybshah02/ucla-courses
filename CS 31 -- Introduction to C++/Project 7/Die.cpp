//
//  Die.cpp
//  BeatThat
//

#include "Die.h"
#include <random>

namespace cs31
{
    
    Die::Die( int sides ) : mSides( sides ), mValue( 1 )
    {

    }
    
    // change the value of the die
    void Die::roll()
    {
        std::random_device rd;
        std::mt19937 e2(rd());
        std::uniform_int_distribution<> dist(1, mSides);
        mValue = dist(e2);
    }
    
    // trivial getter
    int  Die::getValue( ) const
    {
        return( mValue );
    }
    
    // trivial setter
    void Die::setValue( int amount )
    {
        // cheating...
        if (amount <= mSides && amount >= 1)
        {
            mValue = amount;
        }
    }

}
