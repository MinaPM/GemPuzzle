#include "tile.h"
#include <vector>
#include <queue>

std::vector<Tile> closed;
std::priority_queue<Tile> opened;

int main()
{
    int arr[N][N] =
        {
            {1, 3, 4, 8},
            {5, 0, 11, 7},
            {9, 2, 14, 12},
            {13, 6, 10, 15},
        };
    Tile goal = set_goal();
    Tile goal1(goal);
    Tile start(arr);

    if(start==goal){
        std::cout<<"true\n";
    }else
        std::cout<<"false\n";

    // Tile move1(start);
    // opened.push(start);
    // move1.move_up();
    // opened.push(move1);
    // move1.move_left();
    // opened.push(move1);
    // // goal.print_tiles();
    // // start.print_tiles();

    // while (!opened.empty())
    // {
    //     Tile t = opened.top();
    //     t.print_tiles();
    //     opened.pop();
    // }
}