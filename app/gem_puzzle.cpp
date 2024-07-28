#include "Header/tile.h"
#include "Header/barrier.h"

Tile *nodes[4];
bool keepRunning = true;
bool useNestedThreads = false, useMutex = false;
Barrier beforeBarrier(4 + 1), afterBarrier(4 + 1);

std::mutex local_mute;

void filterList3(int id, bool *found, Tile *list, Barrier *before, Barrier *after)
{
    Tile *listItr;
    while (keepRunning)
    {
        before->wait();

        *found = false;
        if (nodes[id] == nullptr)
        {
            *found = false;
        }
        else
        {
            listItr = list;
            while (listItr != nullptr)
            {
                if (nodes[id] == listItr)
                {
                    *found = true;
                }

                listItr = listItr->next;
            }
        }
        after->wait();
    }
}

void filterList2(int id, bool *found, bool *keepRunninglvl2, Tile *list, Barrier *before, Barrier *after)
{
    Tile *listItr;
    while (keepRunning)
    {
        before->wait();
        // *found = nodes[id]->found_in(list);

        *found = false;
        if (nodes[id] == nullptr)
        {
            *found = false;
        }
        else
        {
            listItr = list;
            while (*keepRunninglvl2 && listItr != nullptr)
            {
                if (nodes[id] == listItr)
                {
                    *found = true;
                }

                listItr = listItr->next;
            }
        }

        local_mute.lock();
        *keepRunninglvl2 = false;
        local_mute.unlock();

        after->wait();
    }
}
void filterList(int id, bool *found, Tile *list, Barrier *before, Barrier *after)
{

    while (keepRunning)
    {
        before->wait();
        *found = nodes[id]->found_in(list);

        after->wait();
    }
}

void filter(int id)
{
    Barrier *beforeBarrier2 = new Barrier(2 + 1), *afterBarrier2 = new Barrier(2 + 1);
    std::thread threads[2];
    bool *foundInClosed = new bool, *foundInOpened = new bool, *local_keep_running = new bool;
    if (useNestedThreads)
    {
        if (useMutex)
        {

            threads[0] = std::thread(filterList2, id, foundInOpened, local_keep_running, Tile::opened, beforeBarrier2, afterBarrier2);
            threads[1] = std::thread(filterList2, id, foundInClosed, local_keep_running, Tile::closed, beforeBarrier2, afterBarrier2);
        }
        else
        {
            threads[0] = std::thread(filterList3, id, foundInOpened, Tile::opened, beforeBarrier2, afterBarrier2);
            threads[1] = std::thread(filterList3, id, foundInClosed, Tile::closed, beforeBarrier2, afterBarrier2);
        }
    }

    while (keepRunning)
    {
        beforeBarrier.wait();

        if (useNestedThreads)
        {
            *foundInClosed = false;
            *foundInOpened = false;
            *local_keep_running = true;
            beforeBarrier2->wait();
            afterBarrier2->wait();
            if (*foundInClosed || *foundInOpened)
            {
                delete nodes[id];
                nodes[id] = nullptr;
            }
        }

        else
        {
            if (nodes[id]->is_duplicate())
            {
                delete nodes[id];
                nodes[id] = nullptr;
            }
        }

        afterBarrier.wait();
    }

    if (useNestedThreads)
    {

        beforeBarrier2->wait();
        afterBarrier2->wait();

        threads[0].join();
        threads[1].join();
    }

    delete beforeBarrier2;
    delete afterBarrier2;
    delete foundInClosed;
    delete foundInOpened;
    delete local_keep_running;
}

void expand2(Tile *node)
{

    for (int i = 0; i < 4; i++)
    {
        // 0 up, 1 down, 2 left, 4 right
        // if the move can be made
        if ((node->*moves[0][i])())
        {
            nodes[i] = new Tile(*node);
            nodes[i]->previous = node;
            // make the move
            (nodes[i]->*moves[1][i])();
        }
        else
        {
            delete nodes[i];
            nodes[i] = nullptr;
        }
    }

    beforeBarrier.wait();
    afterBarrier.wait();

    for (int i = 0; i < 4; i++)
        if (nodes[i] != nullptr)
        {
            nodes[i]->insertion_sort();
            nodes[i] = nullptr;
        }
}

/***
 * Expands the node and adds it to the opened list
 */
void expand(Tile *node)
{

    for (int i = 0; i < 4; i++)
    {
        // 0 up, 1 down, 2 left, 4 right
        // if the move can be made
        if ((node->*moves[0][i])())
        {
            Tile *newMove = new Tile(*node);
            newMove->previous = node;
            // make the move
            (newMove->*moves[1][i])();

            // if it is a duplicate
            if (newMove->is_duplicate())
            {
                //  delete the move
                delete newMove;
                newMove = nullptr;
            }
            else
            {
                // add it to be expanded later
                newMove->insertion_sort();
            }
        }
    }
}

void display_list(Tile *node)
{
    int pathlength = 0;
    while (node != nullptr)
    {
        pathlength++;
        node->print_tiles();
        std::cout << "\n";
        node = node->previous;
    }
    std::cout << "\nPath lengh=" << pathlength << std::endl
              << "\n";
}

void display_list_reversed(Tile *node, int *pathlength)
{
    if (node == nullptr)
    {
        std::cout << "\nPath lengh=" << *pathlength << "\n\n";
        return;
    }
    else
    {
        (*pathlength)++;
        display_list_reversed(node->previous, pathlength);
        node->print_tiles();
        std::cout << "\n";
    }
}

int main(int argc, char **argv)
{

    TileType arr[N][N] = {
        {1, 2, 7, 11},
        {3, 6, 10, 4},
        {5, 14, 8, 12},
        {9, 13, 15, 0},
    };

    bool solvable = false;
    Tile goal = set_goal();

    Tile::opened = new Tile(arr);
    int itr = 0;
    Tile *current;

    std::string s;

    bool useThreads = false;

    void (*expandFunction)(Tile *);
    expandFunction = expand;

    if (argc > 1 && argv[1][0] == 'm')
    {
        useThreads = true;
        if (argv[1][0] == 'n')
            useNestedThreads = true;

        if (argv[1][0] == 'm')
            useMutex = true;
    }

    std::thread threads[4];
    if (useThreads)
    {
        for (int i = 0; i < 4; i++)
        {
            threads[i] = std::thread(filter, i);
        }
        expandFunction = expand2;
    }

    while (Tile::opened != NULL)
    {
        current = Tile::opened;
        Tile::opened = Tile::opened->next;
        if (*current == goal)
        {
            int pathlength = 0;
            display_list_reversed(current, &pathlength);
            solvable = true;
            break;
        }

        expandFunction(current);

        itr++;
        if (itr % 10000 == 0)
        {
            std::cout << itr << "\n";
        }
        current->close();
    }

    if (useThreads)
    {
        keepRunning = false;
        beforeBarrier.wait();
        afterBarrier.wait();

        for (int i = 0; i < 4; i++)
        {
            threads[i].join();
        }
    }

    if (!solvable)
        std::cout << "No solution found\n";
}