#include "tile.h"
#include <vector>
#include <queue>

// std::vector<Tile> closed;
// std::vector<Tile> opened;

// struct TileTracker
// {
//     Tile node;
//     TileTracker *previous;
//     TileTracker *next;
// } *opened, *closed, *succ_nodes[4];

Tile *opened, *closed, *succ_nodes[4];

void expand(Tile *node)
{
    // std::cout << "now in expand\n";

    for (int i = 0; i < 4; i++)
    {

        succ_nodes[i] = new Tile(node->tiles); /*(Tile *)malloc(sizeof(Tile));*/
        succ_nodes[i]->g = node->g;
        // *succ_nodes[i] = *node;
        succ_nodes[i]->previous = node;
    }

    if (!succ_nodes[0]->move_up())
    {
        free(succ_nodes[0]);
        succ_nodes[0] = nullptr;
    }

    if (!succ_nodes[1]->move_down())
    {
        free(succ_nodes[1]);
        succ_nodes[1] = nullptr;
    }

    if (!succ_nodes[2]->move_left())
    {
        free(succ_nodes[2]);
        succ_nodes[2] = nullptr;
    }

    if (!succ_nodes[3]->move_right())
    {
        free(succ_nodes[3]);
        succ_nodes[3] = nullptr;
    }

    // for (int i = 0; i < 4; i++)
    // {
    //     succ_nodes[i]->node.print_tiles();
    // }
}

bool found_in(Tile *pnode, Tile *list)
{
    while (list != NULL)
    {
        if (pnode == list)
        {
            return true;
        }
        list = list->next;
    }
    return false;
}

void insertion_sort(Tile *pnode)
{
    Tile *cursor = opened, *prev = opened;

    while (cursor != NULL)
    {
        if (cursor->f > pnode->f)
            break;

        prev = cursor;
        cursor = cursor->next;
    }

    // inserting at head
    if (opened == NULL || cursor == opened)
        opened = pnode;
    // inserting in the middle
    else
        prev->next = pnode;
    if (cursor != NULL)
        pnode->next = cursor;
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
    std::cout << "path length: " << pathlength << std::endl;
}

int main()
{
    int arr[N][N] =
        {
            {5, 6, 1, 3},
            {2, 0, 8, 4},
            {9, 10, 7, 15},
            {13, 14, 12, 11}};
    Tile goal = set_goal();
    Tile start(arr);

    // opened = (Tile *)malloc(sizeof(Tile));
    opened = new Tile();

    *opened = start;
    opened->next = nullptr;
    opened->previous = nullptr;

    Tile *current;
    current = opened;

    while (opened != NULL)
    {
        current = opened;
        opened = opened->next;
        if (*current == goal)
        {
            std::cout << "found\n";
            display_list(current);
            break;
        }
        expand(current);
        for (int i = 0; i < 4; i++)
        {
            if (found_in(succ_nodes[i], opened) ||
                found_in(succ_nodes[i], closed))
            {
                free(succ_nodes[i]);
                succ_nodes[i] = nullptr;
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (succ_nodes[i] != nullptr)
            {
                insertion_sort(succ_nodes[i]);
            }
        }
        current->next = closed;
        closed = current;
    }
}