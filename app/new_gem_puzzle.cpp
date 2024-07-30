#include "Header/Puzzle.h"

int main()
{
    TileType arr[N][N] = {
        {1, 2, 7, 11},
        {3, 6, 10, 4},
        {5, 14, 8, 12},
        {9, 13, 15, 0},
    };

    Puzzle puzzle(arr);
    puzzle.solve();

    puzzle.print_solution();
}