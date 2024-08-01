#include "barrier.h"
#include <random>

#include "TileControls.h"
#include "TileData.h"
#include "TileShape.h"

class Puzzle
{
private:
    int numberOfTiles;
    int gridSize;

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
        foundInOpened,
        solved;

    // for ui
    TileControls &tileControls;
    TileData &tileData;
    TileGrid &tileGrid;

public:
    Puzzle(TileControls &tileControls, TileData &tileData, TileGrid &tileGrid)
        : tileControls(tileControls),
          tileData(tileData),
          tileGrid(tileGrid),
          goal(set_goal()),
          before(3),
          after(3)
    {
        closed_count = 0;
        opened_count = 0;
        solutionSteps = 0;

        solved = false;
        keepRunning = false;
        foundInClosed = false;
        foundInOpened = false;

        closed = nullptr;
        currentNode = nullptr;
        iterator = nullptr;
        solutionPath = nullptr;

        for (auto &direction : Directions)
            expandedNodes[direction] != nullptr;

        opened = new Tile(goal);
        iterator = opened;
        update_UI();
    }

    bool solve(bool multithread = false)
    {
        if (solved)
            return solved;

        if (multithread)
            initialize_threads_resources();

        tileData.resetClock();
        while (opened != nullptr)
        {
            iterator = opened;
            opened = opened->next;

            update_UI();

            std::this_thread::sleep_for(std::chrono::milliseconds(
                100 * (tileControls.sliders[SolvingSpeedSlider].max -
                       tileControls.sliders[SolvingSpeedSlider].current)));

            if (*iterator == goal)
            {
                get_solution();
                solved = true;
                break;
            }

            expand();

            (multithread) ? filter_with_threads() : filter();

            for (auto &direction : Directions)
                if (expandedNodes[direction] != nullptr)
                    insert_to_open(expandedNodes[direction]);

            insert_to_close(iterator);
        }

        clean_up(multithread);
        return solved;
    }

    bool shuffle()
    {
        if (solved)
            return solved;
        if (opened == nullptr)
            return false;

        int intensity = tileControls.sliders[ShuffleSlider].current;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 3);

        while (intensity--)
        {
            if (!opened->move(Directions[distrib(gen)]))
            {
                intensity++;
                continue;
            }
            update_UI();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        opened->direction=NONE;
        return true;
    }

    void display_solution()
    {
        if (!solved)
            return;
        iterator = solutionPath;
        while (iterator != nullptr)
        {
            update_UI();
            std::this_thread::sleep_for(std::chrono::milliseconds(
                100 * (tileControls.sliders[SolutionSpeedSlider].max -
                       tileControls.sliders[SolutionSpeedSlider].current)));
            iterator = iterator->next;
        }
    }
    bool isSolved() { return solved; }

private:
    void update_UI()
    {
        tileGrid.update_values(*iterator);
        tileData.updateData(opened_count, closed_count, iterator->f, iterator->g, iterator->h);
    }

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
            if (iterator->movable(direction))
            {
                expandedNodes[direction] = new Tile(*iterator);
                expandedNodes[direction]->previous = iterator;
                expandedNodes[direction]->move(direction);
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

    bool found_in(Tile *node, Tile *list)
    {
        if (node == nullptr)
            return false;

        while (list != nullptr)
        {
            if (node == list)
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

        for (int i = 0; i < 2 && closed != nullptr; i++)
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
        for (i = 1; i < NxN; i++)
        {
            row = (i - 1) / N;
            col = (i - 1) % N;
            tempgoal[row][col] = i;
        }

        tempgoal[N - 1][N - 1] = 0;
        Tile tgoal(tempgoal);
        return tgoal;
    }
};