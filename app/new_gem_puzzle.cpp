#include "Header/Puzzle.h"

int main(int argc, char **argv)
{
    TileType arr[N][N] = {
        {1, 2, 7, 11},
        {3, 6, 10, 4},
        {5, 14, 8, 12},
        {9, 13, 15, 0},
    };

    Puzzle puzzle(arr);
    
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