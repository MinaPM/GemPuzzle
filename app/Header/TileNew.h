#include <iostream>
#include <iomanip>
#include <string.h>

#define N 4
#define NxN (N * N)
typedef unsigned char TileType;
// higher N values (>15) require a larger data type

enum Direction
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    NONE
};
const Direction Directions[] = {UP, DOWN, LEFT, RIGHT};

class Tile
{
public:
    TileType tiles[N][N], zeroRow, zeroColumn;
    Direction direction;
    int f, g, h, H[N][N];
    Tile *previous, *next;

private:
    /**
     * Default constructor
     */
    Tile()
    {
        g = 0;
        h = 0;
        f = 0;
        direction = NONE;
        previous = nullptr;
        next = nullptr;
    }

public:
    /**
     * Constructor
     *
     * @param tiles the array of tiles
     */
    Tile(TileType tiles[N][N]) : Tile()
    {
        memcpy(this->tiles, tiles, sizeof(TileType) * NxN);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (this->tiles[i][j] == 0)
                {
                    zeroRow = i;
                    zeroColumn = j;
                }
    }

    /**
     * Copy constructor
     *
     * @param tile the tile to be copied
     */
    Tile(const Tile &tile) : Tile()
    {
        memcpy(tiles, tile.tiles, sizeof(TileType) * NxN);
        memcpy(H, tile.H, sizeof(tile.H[0][0]) * NxN);
        direction = tile.direction;
        zeroRow = tile.zeroRow;
        zeroColumn = tile.zeroColumn;
        f = tile.f;
        g = tile.g;
        h = tile.h;
    }

    /**
     * Calculates the Manhattan distance for each tile and add them to h
     */
    void manhattan_distance()
    {
        h = 0;
        int i, j, goalRow, goalColumn;

        for (i = 0; i < N; i++)
            for (j = 0; j < N; j++)
            {
                /*   */ goalRow = (tiles[i][j] == 0) ? N - 1 : (tiles[i][j] - 1) / N;
                /**/ goalColumn = (tiles[i][j] == 0) ? N - 1 : (tiles[i][j] - 1) % N;

                H[i][j] = (abs(i - goalRow) + abs(j - goalColumn));
                if (tiles[i][j] != 0)
                    h += H[i][j];
            }
    }
    /**
     * Updates the fringe (f), cost (g), and heuristic (h)
     */
    void update_fgh()
    {
        manhattan_distance();
        f = h + ++g;
    }

    /**
     * Checks if empty tile can be moved in the direction
     * @param direction the direction to check
     * @return True if the empty tile can be moved in the direction
     */
    bool movable(Direction direction)
    {
        switch (direction)
        {
        case UP:
            return (this->direction != DOWN) && zeroRow > 0;
        case DOWN:
            return (this->direction != UP) && zeroRow < N - 1;
        case LEFT:
            return (this->direction != RIGHT) && zeroColumn > 0;
        case RIGHT:
            return (this->direction != LEFT) && zeroColumn < N - 1;

        default:
            return false;
        }
    }

    /**
     * Moves the empty tile in the direction
     * @param direction the direction to move in
     * @return true if the move is successful
     */
    bool move(Direction direction)
    {
        if (!movable(direction) || direction == NONE)
            return false;

        switch (direction)
        {
        case UP:
            swap_tiles(zeroRow, zeroColumn, zeroRow--, zeroColumn);
            break;
        case DOWN:
            swap_tiles(zeroRow, zeroColumn, zeroRow++, zeroColumn);
            break;
        case LEFT:
            swap_tiles(zeroRow, zeroColumn, zeroRow, zeroColumn--);
            break;
        case RIGHT:
            swap_tiles(zeroRow, zeroColumn, zeroRow, zeroColumn++);
            break;
        }

        this->direction = direction;
        update_fgh();
        return true;
    }

    /**
     * Prints the tiles
     */
    void print_tiles()
    {
        for (int i = 0; i < N; i++)
        {
            for (int k = 0; k < N; k++)
                std::cout << "|----";

            std::cout << "|\n";

            for (int j = 0; j < N; j++)
            {
                std::cout << "|";

                if (tiles[i][j])
                    std::cout << " " << std::setw(2) << (int)tiles[i][j] << " ";
                else
                    std::cout << "||||";

                std::cout << (j < N - 1 ? "" : "|\n");
            }
        }
        for (int k = 0; k < N; k++)
            std::cout << "|----";

        std::cout << "|\n";
        std::cout << "f = " << f << "\tg = " << g << "\th = " << h << std::endl;
    }

    bool operator==(Tile const &tile1) const { return !memcmp(tiles, tile1.tiles, sizeof(TileType) * NxN); }
    bool operator<(Tile const &tile1) const { return f < tile1.f; }
    bool operator>(Tile const &tile1) const { return f > tile1.f; }

private:
    void swap_tiles(int row1, int col1, int row2, int col2)
    {
        std::swap(tiles[row1][col1], tiles[row2][col2]);
    }
};
