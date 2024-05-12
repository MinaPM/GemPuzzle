#include "tile.h"
#include <vector>
#include <queue>

// std::vector<Tile> closed;
// std::vector<Tile> opened;

struct TileTracker
{
    Tile node;
    TileTracker *previous;
    TileTracker *next;
} *opened, *closed, *succ_nodes[4];

void expand(TileTracker *node)
{
    std::cout << "now in expand\n";

    for (int i = 0; i < 4; i++)
    {
        succ_nodes[i] = (struct TileTracker *)malloc(sizeof(TileTracker));
        succ_nodes[i]->node = Tile(node->node);
        succ_nodes[i]->previous = node;
    }

    if (!succ_nodes[0]->node.move_up())
    {
        free(succ_nodes[0]);
        succ_nodes[0] = nullptr;
    }

    if (!succ_nodes[1]->node.move_down())
    {
        free(succ_nodes[1]);
        succ_nodes[1] = nullptr;
    }

    if (!succ_nodes[2]->node.move_left())
    {
        free(succ_nodes[2]);
        succ_nodes[2] = nullptr;
    }

    if (!succ_nodes[3]->node.move_right())
    {
        free(succ_nodes[3]);
        succ_nodes[3] = nullptr;
    }

    // for (int i = 0; i < 4; i++)
    // {
    //     succ_nodes[i]->node.print_tiles();
    // }
}

bool found_in(struct TileTracker *pnode, struct TileTracker *list)
{
    while (list != NULL)
    {
        if (pnode->node == list->node)
        {
            return true;
        }
        list = list->next;
    }
    return false;
}

void insertion_sort(struct TileTracker *pnode)
{
    // opened->next == nullptr ? std::cout << "null\n" : std::cout << "not null\n";
    struct TileTracker *cursor = opened, *prev = opened;

    // cursor->node.print_tiles();
    // cursor = cursor->next;
    // cursor->node.print_tiles();

    // cursor->node.print_tiles();
    // return;
    while (cursor->node.tiles != nullptr)
    {
        std::cout << "now in loop\n";
        if (cursor->node.f > pnode->node.f)
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

int main()
{
    int arr[N][N] =
        {
            {1, 3, 4, 8},
            {5, 0, 11, 7},
            {9, 2, 14, 12},
            {13, 6, 10, 15},
        };
    Tile goal = set_goal();
    Tile start(arr);

    opened = (TileTracker *)malloc(sizeof(TileTracker));
    if (opened == NULL)
    {
        std::cout << "it is null\n";
    }

    opened->node = Tile(start);
    opened->next = nullptr;
    opened->previous = nullptr;

    TileTracker *current;
    current = opened;
    expand(current);
    // succ_nodes[0]->node.print_tiles();
    // succ_nodes[1]->node.print_tiles();
    // succ_nodes[2]->node.print_tiles();
    // succ_nodes[3]->node.print_tiles();

    insertion_sort(succ_nodes[0]);
    insertion_sort(succ_nodes[1]);
    insertion_sort(succ_nodes[2]);
    insertion_sort(succ_nodes[3]);

    // while (opened != NULL)
    // {
    //     current = opened;
    //     opened = opened->next;
    //     if (current->node == goal)
    //     {
    //         std::cout << "found\n";
    //         break;
    //     }
    //     expand(current);
    //     for (int i = 0; i < 4; i++)
    //     {
    //         if (found_in(succ_nodes[i], opened) ||
    //             found_in(succ_nodes[i], closed))
    //         {
    //             free(succ_nodes[i]);
    //             succ_nodes[i] = nullptr;
    //         }
    //     }

    //     for (int i = 0; i < 4; i++)
    //     {
    //         if (succ_nodes[i] != nullptr)
    //         {
    //             insertion_sort(succ_nodes[i]);
    //         }
    //     }
    //     current->next = closed;
    //     closed = current;
    // }
}