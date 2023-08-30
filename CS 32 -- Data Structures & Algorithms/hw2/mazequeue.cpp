#include <stdio.h>
#include <iostream>
#include <queue>

class Coord
{
  public:
    Coord(int r, int c) : m_row(r), m_col(c) {}
    int r() const { return m_row; }
    int c() const { return m_col; }
  private:
    int m_row;
    int m_col;
};

// Return true if there is a path from (sr,sc) to (er,ec)
// through the maze; return false otherwise
bool pathExists(char maze[][10], int sr, int sc, int er, int ec){
    
    std::queue<Coord> cord_queue;
    cord_queue.push(Coord(sr, sc));
    maze[sr][sc] = '*';
    
    while (!cord_queue.empty()){
        Coord current_pos = cord_queue.front();
        std::cout << current_pos.r() << "," << current_pos.c() << std::endl;
        cord_queue.pop();
        if (current_pos.r() == er && current_pos.c() == ec)
            return true;
        //check spot south of current position
        if (maze[current_pos.r() + 1][current_pos.c()] == '.'){
            cord_queue.push(Coord(current_pos.r() + 1, current_pos.c()));
            maze[current_pos.r() + 1][current_pos.c()] = '*';
        }
        //check spot west of current position
        if (maze[current_pos.r()][current_pos.c() - 1] == '.'){
            cord_queue.push(Coord(current_pos.r(), current_pos.c() - 1));
            maze[current_pos.r()][current_pos.c() - 1] = '*';
        }
        //check spot north of current position
        if (maze[current_pos.r() - 1][current_pos.c()] == '.'){
            cord_queue.push(Coord(current_pos.r() - 1, current_pos.c()));
            maze[current_pos.r() - 1][current_pos.c()] = '*';
        }
        //check spot east of current position
        if (maze[current_pos.r()][current_pos.c() + 1] == '.'){
            cord_queue.push(Coord(current_pos.r(), current_pos.c() + 1));
            maze[current_pos.r()][current_pos.c() + 1] = '*';
        }
    }
    
    //didn't reach end coordinate so maze never solved, hence return false
    return false;
}
