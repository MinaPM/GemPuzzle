#include "tile.h"
class Container
{

public:
    // static int count;
    
    Container *next;
    Container *previous;
    Tile node;
    Container();
 
    // sorted insert
    void insertion_sort(Tile Tile)
    {
        struct node *cursor = open, *prev = open;

        while (cursor != NULL)
        {
            if (cursor->f > pnode->f)
                break;

            prev = cursor;
            cursor = cursor->next;
        }

        // inserting at head
        if (open == NULL || cursor == open)
            open = pnode;
        // inserting in the middle
        else
            prev->next = pnode;
        if (cursor != NULL)
            pnode->next = cursor;
    }
    ~Container();
};
