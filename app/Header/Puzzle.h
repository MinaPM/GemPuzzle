#include "TileNew.h"
#include "barrier.h"
#include <thread>
#include <random>

enum Direction
{
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    NONE
};
const Direction Directions[] = {UP, DOWN, LEFT, RIGHT};

bool (Tile::*movable[4])() = {&Tile::up_movable,
                              &Tile::down_movable,
                              &Tile::left_movable,
                              &Tile::right_movable};

bool (Tile::*move[4])() = {&Tile::move_up,
                           &Tile::move_down,
                           &Tile::move_left,
                           &Tile::move_right};

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
    Barrier before, after;
    bool keepRunning,
        foundInClosed,
        foundInOpened;

public:
    Puzzle() : before(3), after(3)
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

    bool solve(bool multithread = false)
    {
        if (multithread)
            initialize_threads_resources();

        while (opened != nullptr)
        {
            iterator = opened;
            opened = opened->next;

            if (*iterator == goal)
            {
                get_solution();

                clean_up(multithread);

                return true;
            }

            expand();

            (multithread) ? filter_with_threads() : filter();

            for (auto &direction : Directions)
                if (expandedNodes[direction] != nullptr)
                    insert_to_open(expandedNodes[direction]);

            insert_to_close(iterator);
        }
        clean_up(multithread);
        return false;
    }

    void shuffle_puzzle(int intensity = 30)
    {
        if (opened == nullptr)
            return;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 3);

        int mv;
        while (intensity--)
        {
            mv = distrib(gen);
            if (!(opened->*move[mv])())
            {
                intensity++;
                continue;
            }
        }
    }

    void print_solution()
    {
        std::cout << "\n Total stpes tried = " << opened_count + closed_count << "\n";
        std::cout << "\n Solution steps = " << solutionSteps << "\n\n";

        Tile *iterator = solutionPath;
        while (iterator != nullptr)
        {
            iterator->print_tiles();
            std::cout << "\n";
            iterator = iterator->next;
        }
    }

private:
    void get_solution()
    {
        Tile *tile = iterator;
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
            if ((iterator->*movable[direction])())
            {
                expandedNodes[direction] = new Tile(*iterator);
                expandedNodes[direction]->previous = iterator;
                (expandedNodes[direction]->*move[direction])();
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

    void filter_with_threads()
    {
        for (auto &direction : Directions)
        {
            currentNode = expandedNodes[direction];
            before.wait();
            after.wait();
            if (foundInOpened || foundInClosed)
            {
                delete expandedNodes[direction];
                expandedNodes[direction] = nullptr;
                currentNode = nullptr;
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

    bool mutual_found_in(Tile *list)
    {
        if (currentNode == nullptr)
            return false;

        bool *foundSomewhere = (list == opened) ? &foundInClosed : &foundInOpened;

        while (list != nullptr && !(*foundSomewhere))
        {
            if (currentNode == list)
                return true;

            list = list->next;
        }
        return false;
    }

    void initialize_threads_resources()
    {
        keepRunning = true;

        threads[0] = std::thread(&Puzzle::worker, this, opened);
        threads[1] = std::thread(&Puzzle::worker, this, closed);
    }

    void free_threads_resources()
    {
        keepRunning = false;
        currentNode = nullptr;

        before.wait();
        after.wait();
        for (int i = 0; i < 2; i++)
            threads[i].join();
    }

    void clean_up(bool multithread)
    {
        if (multithread)
            free_threads_resources();

        Tile *temp;

        while (opened != nullptr)
        {
            temp = opened;
            opened = opened->next;
            delete temp;
        }

        // first two nodes are in the solution
        for (int i = 0; i < 2 && closed->next != nullptr; i++)
            closed = closed->next;

        while (closed != nullptr)
        {
            temp = closed;
            closed = closed->next;
            delete temp;
        }
    }

    void worker(Tile *list)
    {
        bool *found = ((list == opened) ? &foundInOpened : &foundInClosed);

        while (keepRunning)
        {
            before.wait();

            *found = mutual_found_in(list);

            after.wait();
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