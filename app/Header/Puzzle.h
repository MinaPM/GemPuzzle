#include "TileNew.h"
#include "barrier.h"
#include <thread>

enum Direction
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    NONE
};
const Direction Directions[] = {UP, DOWN, LEFT, RIGHT};

bool (Tile::*moves[2][4])() = {{&Tile::up_movable,
                                &Tile::down_movable,
                                &Tile::left_movable,
                                &Tile::right_movable},

                               {&Tile::move_up,
                                &Tile::move_down,
                                &Tile::move_left,
                                &Tile::move_right}};

typedef unsigned char TileType;

class Puzzle
{
private:
    int numberOfTiles;
    int gridSize;
    /// needs fixing
    TileType goal_rows[NxN];
    TileType goal_columns[NxN];
    ///
    Tile goal;
    Tile *opened, *closed;
    Tile *iterator,
        *expandedNodes[4],
        *currentNode,
        *solutionPath;
    int closed_count;
    int opened_count;
    int solutionSteps;

    // for multithreading
    std::thread threads[2];
    Barrier *before, *after;
    bool keepRunning,
        foundInClosed,
        foundInOpened;

public:
    Puzzle()
    {
        goal = set_goal();

        closed_count = 0;
        opened_count = 0;
        solutionSteps = 0;

        keepRunning = false;
        foundInClosed = false;
        foundInOpened = false;

        closed = nullptr;
        opened = nullptr;
        currentNode = nullptr;
        iterator = nullptr;
        solutionPath = nullptr;

        for (auto &direction : Directions)
            expandedNodes[direction] != nullptr;
    }

    Puzzle(TileType tiles[N][N]) : Puzzle() { opened = new Tile(tiles); }

    bool solve()
    {
        while (opened != nullptr)
        {
            iterator = opened;
            opened = opened->next;

            if (*iterator == goal)
            {
                get_solution(iterator);
                return true;
            }

            expand();

            filter();

            for (auto &direction : Directions)
                if (expandedNodes[direction] != nullptr)
                    insert_to_open(expandedNodes[direction]);

            insert_to_close(iterator);
        }
        return false;
    }

    // bool multithread_solve() {}

    void print_solution()
    {
        std::cout << "\nSolution steps = " << solutionSteps << "\n\n";

        Tile *iterator = solutionPath;
        while (iterator != nullptr)
        {
            iterator->print_tiles();
            std::cout << "\n";
            iterator = iterator->next;
        }
    }

private:
    void get_solution(Tile *tile)
    {
        while (tile != nullptr)
        {
            tile->next = solutionPath;
            solutionPath = tile;
            solutionSteps++;

            tile = tile->previous;
        }
    }

    void expand()
    {
        for (auto &direction : Directions)
        {
            if ((iterator->*moves[0][direction])())
            {
                expandedNodes[direction] = new Tile(*iterator);
                expandedNodes[direction]->previous = iterator;
                // make the move
                (expandedNodes[direction]->*moves[1][direction])();
            }
            else
            {
                expandedNodes[direction] = nullptr;
            }
        }
    }

    void filter()
    {
        for (auto &direction : Directions)
        {
            currentNode = expandedNodes[direction];
            if (found_in(opened) || found_in(closed))
            {
                delete expandedNodes[direction];
                expandedNodes[direction] = nullptr;
            }
        }
    }

    void insert_to_open(Tile *tile)
    {
        Tile *cursor = opened, *prev = opened;

        while (cursor != NULL)
        {
            if (*cursor > *tile)
                break;

            prev = cursor;
            cursor = cursor->next;
        }
        // inserting at head
        if (opened == NULL || cursor == opened)
            opened = tile;
        // inserting in the middle
        else
            prev->next = tile;
        if (cursor != NULL)
            tile->next = cursor;

        opened_count++;
    }

    void insert_to_close(Tile *tile)
    {
        tile->next = closed;
        closed = tile;
        closed_count++;
    }

    bool found_in(Tile *list)
    {
        if (currentNode == nullptr)
            return false;

        while (list != nullptr)
        {
            if (currentNode == list)
                return true;

            list = list->next;
        }
        return false;
    }

    bool mutual_found_in(Tile *tile, Tile *list, bool *foundSomewhere)
    {
        if (tile == nullptr)
            return false;

        while (!foundSomewhere && list != nullptr)
        {
            if (tile == list)
                return true;

            list = list->next;
        }
        return false;
    }

    void initialize_threads_resources(Tile *expandedNode)
    {
        before = new Barrier(3);
        after = new Barrier(3);
        *expandedNode = nullptr;

        keepRunning = true;
        foundInOpened = false;
        foundInClosed = false;

        for (int i = 0; i < 2; i++)
        {
            threads[i] = std::thread(
                &Puzzle::worker, this,
                ((i % 2 == 0) ? &foundInOpened : &foundInClosed),
                ((i % 2 == 0) ? &foundInClosed : &foundInOpened),
                ((i % 2 == 0) ? opened : closed));
        }
    }

    void free_threads_resources(Tile *expandedNode)
    {
        keepRunning = false;
        expandedNode = nullptr;

        before->wait();
        after->wait();
        for (int i = 0; i < 2; i++)
            threads[i].join();

        delete before;
        delete after;
    }

    void worker(bool *found, bool *foundSomewhere, Tile *list)
    {
        while (keepRunning)
        {
            before->wait();

            *found = mutual_found_in(iterator, list, foundSomewhere);

            after->wait();
        }
    }

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
};