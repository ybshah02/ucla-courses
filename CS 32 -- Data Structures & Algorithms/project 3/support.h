#ifndef SUPPORT_INCLUDED
#define SUPPORT_INCLUDED

const int MAX_DEPTH = 15;

class BestMove{
public:
    BestMove(int col, int value);
    int getBestCol() { return m_bestCol; }
    int getRating() {return m_rating; }
private:
    int m_bestCol;
    int m_rating;
};

bool isSequence(Scaffold& gameboard, int N, int moveColor, int lastCol, int lastLevel);
int determineWinner(Scaffold& gameboard, int N, int lastCol, int& winner, int turnCount);

#endif /* support_h */
