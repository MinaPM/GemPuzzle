#include <iostream>
#include <iomanip>
#include <string.h>
#include <thread>

#define N 4
#define NxN (N * N)
// higher N values (>15) require a larger data type
typedef unsigned char TileType;

class Tile
{
public:
    static TileType goal_rows[NxN];
    static TileType goal_columns[NxN];
    static Tile *opened;
    static Tile *closed;
    static int closed_count;
    static int opened_count;

    TileType tiles[N][N];
    TileType zero_row;
    TileType zero_column;
    int f, g, h, H[N][N];
    class Tile *previous;
    class Tile *next;

    /**
     * Default constructor
     */
    Tile()
    {
        g = 0;
        h = 0;
        f = 0;
        previous = nullptr;
        next = nullptr;
    }

    /**
     * Constructor
     *
     * @param tiles the array of tiles
     */
    Tile(TileType tiles[N][N])
    {
        memcpy(this->tiles, tiles, sizeof(TileType) * NxN);
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
        previous = nullptr;
        next = nullptr;
    }

    /**
     * Copy constructor
     *
     * @param tile the tile to be copied
     */
    Tile(const Tile &tile)
    {
        memcpy(tiles, tile.tiles, sizeof(TileType) * NxN);
        memcpy(H, tile.H, sizeof(tile.H[0][0]) * NxN);
        zero_row = tile.zero_row;
        zero_column = tile.zero_column;
        f = tile.f;
        g = tile.g;
        h = tile.h;
        previous = nullptr;
        next = nullptr;
    }

    /**
     * Sets the order of tiles based on a 2D array
     *
     * @param tiles the array of tiles
     */
    void set_array(TileType tiles[N][N])
    {
        memcpy(this->tiles, tiles, sizeof(TileType) * NxN);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                if (this->tiles[i][j] == 0)
                {
                    zero_row = i;
                    zero_column = j;
                }
    }
    /**
     * Inserts the node in the currently opened node to be expanded later
     */
    void insertion_sort()
    {
        opened_count++;
        Tile *cursor = opened, *prev = opened;

        while (cursor != NULL)
        {
            if (*cursor > *this)
                break;

            prev = cursor;
            cursor = cursor->next;
        }

        // inserting at head
        if (opened == NULL || cursor == opened)
            opened = this;
        // inserting in the middle
        else
            prev->next = this;
        if (cursor != NULL)
            next = cursor;
    }

    /**
     * Closes the node by inserting it to the closed nodes list
     */
    void close()
    {
        closed_count++;
        next = Tile::closed;
        Tile::closed = this;
    }
    /**
     * Calculates the Manhattan distance for each tile and add them to h
     */
    void manhattan_distance()
    {
        h = 0;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                H[i][j] = (abs(i - goal_rows[tiles[i][j]]) + abs(j - goal_columns[tiles[i][j]]));
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
     * @return True if the empty tile can be moved up
     */
    bool up_movable() { return zero_row > 0; }

    /**
     * @return True if the empty tile can be moved down
     */
    bool down_movable() { return zero_row < N - 1; }

    /**
     * @return True if the empty tile can be moved left
     */
    bool left_movable() { return zero_column > 0; }

    /**
     * @return True if the empty tile can be moved right
     */
    bool right_movable() { return zero_column < N - 1; }

    /**
     * Moves the empty tile up
     * @return true if the move is successful
     */
    bool move_up()
    {
        if (!up_movable())
            return false;

        swap_tiles(zero_row, zero_column, zero_row--, zero_column);
        update_fgh();
        return true;
    }

    /**
     * Moves the empty tile down
     * @return true if the move is successful
     */
    bool move_down()
    {
        if (!down_movable())
            return false;

        swap_tiles(zero_row, zero_column, zero_row++, zero_column);
        update_fgh();
        return true;
    }

    /**
     * Moves the empty tile left
     * @return true if the move is successful
     */
    bool move_left()
    {
        if (!left_movable())
            return false;

        swap_tiles(zero_row, zero_column, zero_row, zero_column--);
        update_fgh();
        return true;
    }

    /**
     * Moves the empty tile right
     * @return true if the move is successful
     */
    bool move_right()
    {
        if (!right_movable())
            return false;

        swap_tiles(zero_row, zero_column, zero_row, zero_column++);
        update_fgh();
        return true;
    }

    /**
     * Checks if the tile is found in the list
     *
     * @param list the list to be checked (opened or closed)
     * @return true if the tile is found in the list
     */
    bool found_in(Tile *list)
    {
        if (this == nullptr)
            return false;

        while (list != nullptr)
        {
            if (this == list)
                return true;

            list = list->next;
        }
        return false;
    }
    
    /**
     * Checks if the tile is found in Opened list
     *
     * @param flag true if tile is found
     */
    void found_in_opened(bool *flag)
    {
        if (this == nullptr)
        {
            *flag = false;
            return;
        }
        Tile *list = opened;

        while (list != nullptr)
        {
            if (this == list)
            {
                *flag = true;
                return;
            }

            list = list->next;
        }
        *flag = false;
    }

    /**
     * Checks if the tile is found in Closed list
     *
     * @param flag true if tile is found
     */
    void found_in_closed(bool *flag)
    {
        if (this == nullptr)
        {
            *flag = false;
            return;
        }
        Tile *list = closed;

        while (list != nullptr)
        {
            if (this == list)
            {
                *flag = true;
                return;
            }

            list = list->next;
        }
        *flag = false;
    }
    
    /**
     * Checks if the tile is duplicate
     *
     * @param use_threads determines weather or not threads are to be used
     */
    bool is_duplicate(bool use_threads = false)
    {
        bool found_in_closed = false, found_in_opened = false;
        if (use_threads)
        {
           std::thread openThread = std::thread(Tile::found_in_opened, this, &found_in_opened);
           std::thread closedThread = std::thread(Tile::found_in_closed, this, &found_in_closed);
            openThread.join();
            closedThread.join();
        }
        else
        {
            this->found_in_closed(&found_in_closed);
            this->found_in_opened(&found_in_opened);
        }
        return found_in_closed || found_in_opened;
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

/**
 * Initializes the starting state of the puzzle using the command line arguments
 * @param argv the command line arguments
 * @return the starting state of the puzzle
 */
Tile initialize(char **argv)
{
    TileType tempgoal[N][N], i, j, k, col, row, index = 1;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            tempgoal[i][j] = atoi(argv[index++]);

    return Tile(tempgoal);
}

TileType Tile::goal_rows[NxN];
TileType Tile::goal_columns[NxN];
int Tile::closed_count = 0;
int Tile::opened_count = 0;

Tile *Tile::opened = nullptr;
Tile *Tile::closed;

// function pointers to each move
bool (Tile::*moves[2][4])() = {{&Tile::up_movable,
                                &Tile::down_movable,
                                &Tile::left_movable,
                                &Tile::right_movable},

                               {&Tile::move_up,
                                &Tile::move_down,
                                &Tile::move_left,
                                &Tile::move_right}};

/**
 * Sets the goal state of the puzzle
 * @return the goal state of the puzzle
 */
Tile set_goal()
{
    TileType tempgoal[N][N], i, col, row;
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