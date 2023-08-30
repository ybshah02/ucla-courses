// Scaffold.cpp

#include "provided.h"
#include <iostream>
#include <vector>
#include <stack>

using namespace std;

class ScaffoldImpl
    {
public:
    ScaffoldImpl(int nColumns, int nLevels); 
    int cols() const;
    int levels() const;
    int numberEmpty() const;
    int checkerAt(int column, int level) const; 
    void display() const; 
    bool makeMove(int column, int color); 
    int undoMove();
private:
    int m_cols;
    int m_levels;
    vector<vector<int>> m_grid;
    stack<int> history_cols;
    stack<int> history_levels;
};

ScaffoldImpl::ScaffoldImpl(int nColumns, int nLevels)
{
    //check vals of columns and levels param
    if (nColumns <= 0 || nLevels <= 0){
        cerr << "Error: columns or levels less than zero" << endl;
        exit(0);
    }
    
    //assign vals to private vars
    m_cols = nColumns;
    m_levels = nLevels;
    
    //size grid
    m_grid.resize(m_levels);
    
    for (int i = 0; i < m_grid.size(); i++){
        //size grid colums
        m_grid[i].resize(m_cols);

        //fill grid with vacant spots
        for (int j = 0; j < m_grid[i].size(); j++){
            m_grid[i][j] = VACANT;
        }
    }
}

int ScaffoldImpl::cols() const
{
    return m_cols;
}

int ScaffoldImpl::levels() const
{
    return m_levels;
}

int ScaffoldImpl::numberEmpty() const
{
    int countEmpty = 0;
    
    //iterate through scaffold
    for (int i = 0; i < m_grid.size(); i++){
        for (int j = 0; j < m_grid[i].size(); j++){
            //increment every empty space
            if (m_grid[i][j] == VACANT)
                countEmpty++;
        }
    }
    
    return countEmpty;
}

int ScaffoldImpl::checkerAt(int column, int level) const
{
    //validate input column
    if (column < 1 || column > m_cols)
        return VACANT;
    
    //validate input level
    if (level < 1 || level > m_levels)
        return VACANT;
    
    //return grid at spot level, column
    return m_grid[level-1][column-1];
}

void ScaffoldImpl::display() const{
    //iterature through levels
    for (int i = m_levels-1; i >= 0; i--){
        //add border to the left
        cout << '|';
        for (int j = 0; j < m_cols; j++)
        {
            //print token
            if(m_grid[i][j] == RED){
                cout << 'R';
            }
            else if(m_grid[i][j] == BLACK){
                cout << 'B';
            }
            else if(m_grid[i][j] == VACANT){
                cout << ' ';
            }
            //add border to the right side
            cout << '|';
        }
        cout << endl;
    }
    
    //draw bottom row of grid
    for (int k = 0; k < m_cols; k++)
        cout << '+' << '-';
    
    cout << '+' << endl;
}

bool ScaffoldImpl::makeMove(int column, int color)
{
    //validate columns
    if (column < 1 || column > m_cols){
        return false;
    }
    //validate color
    if (color != RED && color != BLACK){
        return false;
    }
    //check last level of column - if full then assume column is full
    if (m_grid[m_levels-1][column-1] != VACANT)
        return false;
    
    int colLevel = 0;
    //find first vacant spot in the column
    while (m_grid[colLevel][column-1] != VACANT){
        if (colLevel < m_levels - 1)
            colLevel++;
    }
    
    //add token to first vacant spot in column if column not filled
    if (colLevel < m_levels){
        m_grid[colLevel][column-1] = color;
        //track moves in stack
        history_levels.push(colLevel);
        history_cols.push(column-1);
        return true;
    }else
        return false;
}

int ScaffoldImpl::undoMove()
{
    //validate there are moves to undo
    if (history_levels.size() == 0 || history_cols.size() == 0)
        return 0;
    
    int undo_level = history_levels.top();
    int undo_col = history_cols.top();
    
    //set the latest move spot to vacant
    m_grid[undo_level][undo_col] = VACANT;
    
    //delete latest moves from stack after undoing
    history_levels.pop();
    history_cols.pop();
    
    return undo_col + 1;
    
}

//******************** Scaffold functions *******************************

//  These functions simply delegate to ScaffoldImpl's functions.
//  You probably don't want to change any of this code. 

Scaffold::Scaffold(int nColumns, int nLevels)
{
    m_impl = new ScaffoldImpl(nColumns, nLevels);
}
 
Scaffold::~Scaffold()
{
    delete m_impl;
}
 
Scaffold::Scaffold(const Scaffold& other)
{
    m_impl = new ScaffoldImpl(*other.m_impl);
}
 
Scaffold& Scaffold::operator=(const Scaffold& rhs)
{
    if (this != &rhs)
    {
        Scaffold temp(rhs);
        swap(m_impl, temp.m_impl);
    }
    return *this;
}
 
int Scaffold::cols() const
{
    return m_impl->cols();
}

int Scaffold::levels() const
{
    return m_impl->levels();
}

int Scaffold::numberEmpty() const
{
    return m_impl->numberEmpty();
}

int Scaffold::checkerAt(int column, int level) const
{
    return m_impl->checkerAt(column, level);
}
 
void Scaffold::display() const
{
    m_impl->display();
}
 
bool Scaffold::makeMove(int column, int color)
{
    return m_impl->makeMove(column, color);
}
 
int Scaffold::undoMove()
{
    return m_impl->undoMove();
}
