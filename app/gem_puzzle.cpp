#include "tile.h"

std::thread movesThreads[4];

bool use_threads;

Tile *temp_tiles[4];

void createMove(Tile *node, int i)
{
    if (!(node->*moves[0][i])())
    {
        temp_tiles[i] = nullptr;
        return;
    }

    temp_tiles[i] = new Tile(*node);
    temp_tiles[i]->previous = node;
    (temp_tiles[i]->*moves[1][i])();

    // if it is a duplicate
    if (temp_tiles[i]->is_duplicate(use_threads))
    {
        //  delete the move
        delete temp_tiles[i];
        temp_tiles[i] = nullptr;
    }
    // else
    // {
    //     // add it to be expanded later
    //     newMove->insertion_sort();
    // }
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
            if (newMove->is_duplicate(use_threads))
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

void expand_with_threads(Tile *node)
{

    for (int i = 0; i < 4; i++)
    {
        movesThreads[i] = std::thread(createMove, node, i);
    }
    for (int i = 0; i < 4; i++)
    {
        if (movesThreads[i].joinable())
            movesThreads[i].join();
    }
    for (int i = 0; i < 4; i++)
    {
        if (temp_tiles[i] != nullptr)
            temp_tiles[i]->insertion_sort();
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
    /*options

    0 no threads

    1. two threads in every node
        two threads running at a time to check if the
        node is duplicate on the Opened & Closed lists

    2. four threads (or less)
        four threads running to check if every new
        expantion is a duplicate. Opened & closed lists are checked in the same thread

    3. four threads (or less) with two extra threads for Opened & Closed
        same as 2 except Opened & Closed are checked with two seperate threads

    */
    // for multi threads
    int use_threads_int;
    if (argc > 1)
    {
        use_threads_int = atoi(argv[1]);
    }
    else{
        use_threads_int=0;
    }
    //////////////

    void (*expandFunction)(Tile *);
    switch (use_threads_int)
    {
    case 0:
        use_threads = false;
        expandFunction = expand;
        break;
    case 1:
        use_threads = true;
        expandFunction = expand;
        break;
    case 2:
        use_threads = false;
        expandFunction = expand_with_threads;
        break;
    case 3:
        use_threads = true;
        expandFunction = expand_with_threads;
        break;

    default:
        use_threads = false;
        expandFunction = expand;
        break;
    }

    bool solvable = false;
    Tile goal = set_goal();

    Tile::opened = new Tile(arr);
    int itr = 0;
    Tile *current;

    while (Tile::opened != NULL)
    {
        current = Tile::opened;
        Tile::opened = Tile::opened->next;
        if (*current == goal)
        {   
            //uncomment later
            // int pathlength = 0;
            // display_list_reversed(current, &pathlength);
            // solvable = true;
            //comment later
            std::cout<<"done!\n";
            break;

        }

        expandFunction(current);
        // if (use_threads)
        // {
        //     expand_with_threads(current);
        // }
        // else
        // {
        //     expand(current);
        // }


        // itr++;
        // if (itr % 10000 == 0)
        // {
        //     std::cout << itr << "\n";
        // }
        current->close();
    }
    if (!solvable)
        std::cout << "No solution found\n";
}