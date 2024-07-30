#include "Header/Puzzle.h"
#include <fstream>
#include <string>

int main(int argc, char **argv)
{
    TileType arr[N][N] = {
        {1, 2, 7, 11},
        {3, 6, 10, 4},
        {5, 14, 8, 12},
        {9, 13, 15, 0},
    };

    Puzzle puzzle(arr);
    Puzzle puzzle2;
    
    

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

    std::cout << "clean called\n";
    puzzle.clean();
    puzzle.print_solution();

}