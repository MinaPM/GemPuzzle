#include <iostream>
#include <iomanip>
#include <string.h>

#define N 4
#define NxN (N * N)

enum moves
{
    up,
    down,
    left,
    right
};

class Tile
{
public:
    static int goal_rows[NxN];
    static int goal_columns[NxN];

    int tiles[N][N];
    int zero_row;
    int zero_column;
    int f, g, h;

    Tile(int tiles[N][N])
    {
        memcpy(this->tiles, tiles, sizeof(int) * NxN);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (this->tiles[i][j] == 0)
                {
                    zero_row = i;
                    zero_column = j;
                }

        g = 0;
        h = 0;
        f = 0;
    }

    Tile(const Tile &tile)
    {
        memcpy(tiles, tile.tiles, sizeof(int) * NxN);
        zero_row = tile.zero_row;
        zero_column = tile.zero_column;
        f = tile.f;
        g = tile.g;
        h = tile.h;
    }

    void manhattan_distance()
    {
        h = 0;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (tiles[i][j] != 0)
                    h += (abs(i - goal_rows[tiles[i][j]]) + abs(j - goal_columns[tiles[i][j]]));
    }

    void update_fgh()
    {
        manhattan_distance();
        f = h + ++g;
    }

    bool up_movable() { return zero_row > 0; }
    bool down_movable() { return zero_row < N - 1; }
    bool left_movable() { return zero_column > 0; }
    bool right_movable() { return zero_column < N - 1; }

    void move_up()
    {
        if (up_movable())
        {
            swap_tiles(zero_row, zero_column, zero_row--, zero_column);
            update_fgh();
        }
    }
    void move_down()
    {
        if (down_movable())
        {
            swap_tiles(zero_row, zero_column, zero_row++, zero_column);
            update_fgh();
        }
    }
    void move_left()
    {
        if (left_movable())
        {
            swap_tiles(zero_row, zero_column, zero_row, zero_column--);
            update_fgh();
        }
    }
    void move_right()
    {
        if (right_movable())
        {
            swap_tiles(zero_row, zero_column, zero_row, zero_column++);
            update_fgh();
        }
    }

    void print_tiles()
    {
        for (int i = 0; i < N; i++)
        {
            for (int k = 0; k < N; k++)
            {
                std::cout << "|----";
            }
            std::cout << (i == 0 ? "|\n" : "|\n");

            for (int j = 0; j < N; j++)
            {
                if (tiles[i][j])
                    std::cout << "| " << std::setw(2) << tiles[i][j] << (j < N - 1 ? " " : " |\n");
                else
                {
                    std::cout << "|" << "||||" << (j < N - 1 ? "" : "|\n");
                }
            }
        }
        for (int k = 0; k < N; k++)
        {
            std::cout << "|----";
        }
        std::cout << "|\n";
        std::cout << "f = " << f << "\tg = " << g << "\th = " << h << std::endl;
    }
    bool operator==(Tile const &tile1) const { return !memcmp(tiles, tile1.tiles, sizeof(int) * NxN); }

    // this reversed intentionally i dont know how to reverse it
    bool operator<(Tile const &tile1) const { return f > tile1.f; }

private:
    void swap_tiles(int row1, int col1, int row2, int col2)
    {
        std::swap(tiles[row1][col1], tiles[row2][col2]);
    }
};

Tile initialize(char **argv)
{
    int tempgoal[N][N], i, j, k, col, row, index = 1;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            tempgoal[i][j] = atoi(argv[index++]);

    return Tile(tempgoal);
}

int Tile::goal_rows[NxN];
int Tile::goal_columns[NxN];

Tile set_goal()
{
    int tempgoal[N][N], i, col, row;
    Tile::goal_rows[0] = N - 1;
    Tile::goal_columns[0] = N - 1;
    for (i = 1; i < NxN; i++)
    {
        row = (i - 1) / N;
        col = (i - 1) % N;
        Tile::goal_rows[i] = row;
        Tile::goal_columns[i] = col;
        tempgoal[row][col] = i;
    }
    tempgoal[N - 1][N - 1] = 0;
    Tile tgoal(tempgoal);
    return tgoal;
}