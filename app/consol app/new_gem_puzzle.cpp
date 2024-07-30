#include "../Header/Puzzle.h"

int main(int argc, char **argv)
{
    TileType arr[N][N] = {
        {1, 2, 7, 11},
        {3, 6, 10, 4},
        {5, 14, 8, 12},
        {9, 13, 15, 0},
    };
      TileType arr2[N][N] = {
        {15, 13, 4, 14},
        {12, 3, 5, 0},
        {1, 7, 11, 8},
        {9, 2, 10, 6},
    };

    Puzzle puzzle(arr2);
    
    if (argc > 1)
    {
        switch (argv[1][0])
        {
        case '2':
            puzzle.solve(true);
            break;

        case '1':
        default:
            puzzle.solve();
            break;
        }
    }
    else
    {
        puzzle.solve();
    }

    puzzle.print_solution();

}