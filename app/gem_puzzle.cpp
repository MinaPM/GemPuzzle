#include "Header/tile.h"
#include "Header/barrier.h"

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

void display_list(Tile *node, int &pathlength)
{
    if (node == nullptr)
    {
        std::cout << "\nPath lengh=" << pathlength << "\n\n";
        return;
    }
    else
    {
        (pathlength)++;
        display_list(node->previous, pathlength);
        node->print_tiles();
        std::cout << "\n";
    }
}

void worker(bool *found, bool *foundSomewhere, bool *keepRunning,
            Barrier *before, Barrier *after,
            Tile *currentNode, Tile *list)
{
    while (*keepRunning)
    {
        before->wait();

        *found = currentNode->mutual_found_in(list, foundSomewhere);

        after->wait();
    }
}

bool solve_with_2_threads()
{
    Tile goal = set_goal();

    Tile *current, *expandedNode = nullptr;
    bool keepRunning = true;

    bool foundInOpened = false,
         foundInClosed = false;

    Barrier before(3), after(3);
    std::thread threads[2];
    for (int i = 0; i < 2; i++)
    {
        threads[i] = std::thread(
            worker,
            ((i % 2 == 0) ? &foundInOpened : &foundInClosed),
            ((i % 2 == 0) ? &foundInClosed : &foundInOpened),
            &keepRunning,
            &before, &after,
            expandedNode,
            ((i % 2 == 0) ? Tile::opened : Tile::closed));
    }
    while (Tile::opened != NULL)
    {
        current = Tile::opened;
        Tile::opened = Tile::opened->next;
        if (*current == goal)
        {
            int pathlength = 0;
            display_list(current, pathlength);

            expandedNode = nullptr;
            keepRunning = false;

            before.wait();
            after.wait();
            for (int i = 0; i < 2; i++)
                threads[i].join();

            return true;
        }
        for (auto &direction : Directions)
        {
            if ((current->*moves[0][direction])())
            {
                expandedNode = new Tile(*current);
                expandedNode->previous = current;
                (expandedNode->*moves[1][direction])();

                before.wait();
                after.wait();

                if (expandedNode != nullptr && !(foundInOpened || foundInClosed))
                    expandedNode->insertion_sort();
            }
            else
            {
                expandedNode = nullptr;
            }
        }

        current->close();
    }

    keepRunning = false;
    expandedNode = nullptr;

    before.wait();
    after.wait();
    for (int i = 0; i < 2; i++)
        threads[i].join();
    return false;
}

bool solve()
{
    Tile goal = set_goal();
    int itr = 0;
    Tile *current;

    while (Tile::opened != NULL)
    {
        current = Tile::opened;
        Tile::opened = Tile::opened->next;
        if (*current == goal)
        {
            int pathlength = 0;
            display_list(current, pathlength);
            return true;
        }

        expand(current);

        current->close();
    }

    return false;
}

int main(int argc, char **argv)
{

    TileType arr[N][N] = {
        {1, 2, 7, 11},
        {3, 6, 10, 4},
        {5, 14, 8, 12},
        {9, 13, 15, 0},
    };

    bool solved = false;

    Tile::opened = new Tile(arr);
    if (argc > 1)
    {
        switch (argv[1][0])
        {
        case '2':
            solved = solve_with_2_threads();
            break;

        case '1':
        default:
            solved = solve();
            break;
        }
    }
    else
    {
        solved = solve();
    }

    if (!solved)
        std::cout << "No solution found\n";
}