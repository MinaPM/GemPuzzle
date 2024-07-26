#include "tile.h"
#include "barrier.h"

Tile *nodes[4];
bool keepRunning = true;
Barrier beforeBarrier(4 + 1), afterBarrier(4 + 1);

void filter(int id)
{
    while (keepRunning)
    {
        beforeBarrier.wait();

        if (nodes[id]->is_duplicate())
        {
            delete nodes[id];
            nodes[id] = nullptr;
        }

        afterBarrier.wait();
    }
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