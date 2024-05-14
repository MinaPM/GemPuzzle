#include "tile.h"

/***
 * 
*/
void expand(Tile *node)
{
    for (int i = 0; i < 4; i++)
    {
        // 0 up, 1 down, 2 left, 4 right
        // of the move can be made
        if ((node->*moves[0][i])())
        {
            Tile *newMove = new Tile(*node);
            newMove->previous = node;
            (newMove->*moves[1][i])();

            // if it is a duplicate found in opened or closed
            if (newMove->found_in(Tile::opened) ||
                newMove->found_in(Tile::closed))
            { // delete the move
                delete newMove;
                newMove = nullptr;
            }
            else
            { // add it to be expanded later
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

int main()
{
    int arr[N][N] = {{5, 6, 1, 3},
                     {2, 0, 8, 4},
                     {9, 10, 7, 15},
                     {13, 14, 12, 11}};

    bool solvable = false;
    Tile goal = set_goal();

    Tile::opened = new Tile(arr);

    Tile *current;

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
        expand(current);

        current->close_node();
    }
    if (!solvable)
        std::cout << "No solution found\n";
}