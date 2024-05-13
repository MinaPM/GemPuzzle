#include "tile.h"

// Tile *opened, *closed, *succ_nodes[4];
Tile *succ_nodes[4];

void expand(Tile *node)
{
    for (int i = 0; i < 4; i++)
    { // 0 up, 1 down, 2 left, 4 right
        succ_nodes[i] = new Tile(*node);
        succ_nodes[i]->previous = node;
        if (!(succ_nodes[i]->*moves[i])())
        {
            delete succ_nodes[i];
            succ_nodes[i] = nullptr;
        }
    }
}

bool found_in(Tile *pnode, Tile *list)
{
    if (pnode == nullptr)
        return false;

    while (list != nullptr)
    {
        if (pnode == list)
            return true;

        list = list->next;
    }
    return false;
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
    std::cout << "Path lengh=" << pathlength << std::endl;
}

void display_list_reversed(Tile *node, int *pathlength)
{
    if (node == nullptr)
    {
        std::cout << "Path lengh=" << *pathlength << "\n";
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

    Tile goal = set_goal();
    Tile start(arr);

    Tile::opened = new Tile();

    *Tile::opened = start;
    Tile::opened->next = nullptr;
    Tile::opened->previous = nullptr;

    Tile *current;
    current = Tile::opened;

    while (Tile::opened != NULL)
    {
        current = Tile::opened;
        Tile::opened = Tile::opened->next;
        if (*current == goal)
        {
            int pathlength = 0;
            display_list_reversed(current, &pathlength);
            break;
        }
        expand(current);
        for (int i = 0; i < 4; i++)
        {
            if (found_in(succ_nodes[i], Tile::opened) ||
                found_in(succ_nodes[i], Tile::opened))
            {
                delete succ_nodes[i];
                succ_nodes[i] = nullptr;
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (succ_nodes[i] != nullptr)
            {
                succ_nodes[i]->insertion_sort();
                // insertion_sort(succ_nodes[i]);
            }
        }
        current->next = Tile::closed;
        Tile::closed = current;
    }
}