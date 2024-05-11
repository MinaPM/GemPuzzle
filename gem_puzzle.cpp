#include <iostream>
#include "tile.h"

int main()
{

    int arr[N][N] =
        {
            {1, 3, 4, 8},
            {5, 0, 11, 7},
            {9, 2, 14, 12},
            {13, 6, 10, 15},
        }

    ;
    Tile goal = set_goal();
    Tile start(arr);
    start.print_tiles();
}