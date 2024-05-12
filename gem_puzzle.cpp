#include "tile.h"

Tile *opened, *closed, *succ_nodes[4];
// Tile *succ_nodes[4];

void expand(Tile *node)
{
    for (int i = 0; i < 4; i++)
    {
        succ_nodes[i] = new Tile(*node);
        succ_nodes[i]->previous = node;
    }

    if (!succ_nodes[0]->move_up())
    {
        delete succ_nodes[0];
        succ_nodes[0] = nullptr;
    }

    if (!succ_nodes[1]->move_down())
    {
        delete succ_nodes[1];
        succ_nodes[1] = nullptr;
    }

    if (!succ_nodes[2]->move_left())
    {
        delete succ_nodes[2];
        succ_nodes[2] = nullptr;
    }

    if (!succ_nodes[3]->move_right())
    {
        delete succ_nodes[3];
        succ_nodes[3] = nullptr;
    }
}

bool found_in(Tile *pnode, Tile *list)
{
    if (pnode == nullptr)
        return false;

    while (list != nullptr)
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
        if (*cursor > *pnode)
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
            int pathlength = 0;
            display_list_reversed(current, &pathlength);
            break;
        }
        expand(current);
        for (int i = 0; i < 4; i++)
        {
            if (found_in(succ_nodes[i], opened) ||
                found_in(succ_nodes[i],opened))
            {
                delete succ_nodes[i];
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