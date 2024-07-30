//
//  RandomNumber.h
//  BeatThat
//
//

#ifndef RandomNumber_h
#define RandomNumber_h

// This class is completely finished and there is nothing for a CS 31 student to do here.
// The purpose of this class is to provide random numbers within a specified range
class RandomNumber
{
public:
    // supply a range of value between min and max inclusive
    // by default, the min and max value are in the desired range
    RandomNumber( int min, int max, bool minInclusive = true, bool maxInclusive = true );
    // pull a random number in the desired range
    int random( );
    
private:
    int mMinimum;  // the smallest random number desired
    int mMaximum;  // the largest random number desired
};


#endif 
