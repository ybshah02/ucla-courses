//
//  RandomNumber.cpp
//  BeatThat
//
//

#include "RandomNumber.h"


#include <random>
#include <utility>

using namespace std;

// supply a range of value between min and max inclusive
// by default, the min and max value are in the desired range
RandomNumber::RandomNumber( int min, int max,
                           bool minInclusive, bool maxInclusive )
: mMinimum( min ), mMaximum( max )
{
    if (mMinimum > mMaximum)
    {
        swap( mMinimum, mMaximum );
    }
    if (!minInclusive)
    {
        mMinimum++;
    }
    if (!maxInclusive)
    {
        mMaximum--;
    }
}

// pull a random number in the desired range
int RandomNumber::random( )
{
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro( mMinimum, mMaximum );
    
    return( distro( generator ) );
}
