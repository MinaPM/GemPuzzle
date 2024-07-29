#include "Header/tile.h"
#include "Header/barrier.h"

/***
 * Expands the node and adds it to the opened list
 */
void expand(Tile *node)
{
    for (auto &direction : Directions)
    {
        // if the move can be made
        if ((node->*moves[0][direction])())
        {
            Tile *newMove = new Tile(*node);
            newMove->previous = node;
            // make the move
            (newMove->*moves[1][direction])();

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
        pathlength++;
        display_list(node->previous, pathlength);
        node->print_tiles();
        std::cout << "\n";
    }
}

bool solve()
{

    int itr = 0;
    Tile *current;
    Tile goal = set_goal();

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

        itr++;
        if (itr % 10000 == 0)
        {
            std::cout << itr << "\n";
        }
        current->close();
    }

    return false;
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

    Barrier before(2), after(2);
    std::thread thread1(
        worker,
        &foundInClosed, &foundInOpened, &keepRunning,
        &before, &after,
        expandedNode, Tile::closed);

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
            thread1.join();

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
                foundInOpened = expandedNode->mutual_found_in(Tile::opened, &foundInClosed);
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
    thread1.join();

    return false;
}

bool solve_with_2_threads2()
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

void expand2(Tile *node, Tile *expandedNodes[])
{
    for (auto &direction : Directions)
    {
        if ((node->*moves[0][direction])())
        {
            expandedNodes[direction] = new Tile(*node);
            expandedNodes[direction]->previous = node;
            (expandedNodes[direction]->*moves[1][direction])();
        }
        else
        {
            expandedNodes[direction] = nullptr;
        }
    }
}

bool solve_with_n_threads()
{
    Tile goal = set_goal();

    Tile *current, *expandedNodes[4];
    bool keepRunning = true;

    bool foundInOpened[4],
        foundInClosed[4];

    Barrier before(9), after(9);
    std::thread threads[8];

    for (int i = 0; i < 4; i++)
        expandedNodes[i] = nullptr;

    for (int i = 0; i < 8; i++)
    {
        int j = i / 2;
        threads[i] = std::thread(
            worker,
            ((i % 2 == 0) ? &foundInOpened[j] : &foundInClosed[j]),
            ((i % 2 == 0) ? &foundInClosed[j] : &foundInOpened[j]),
            &keepRunning, &before, &after, expandedNodes[j],
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

            keepRunning = false;
            for (int i = 0; i < 4; i++)
            {
                expandedNodes[i] = nullptr;
            }

            before.wait();
            after.wait();

            for (int i = 0; i < 8; i++)
            {
                threads[i].join();
            }

            return true;
        }

        expand2(current, expandedNodes);

        before.wait();
        after.wait();

        for (int i = 0; i < 4; i++)
        {

            if (expandedNodes[i] != nullptr && !(foundInOpened[i] || foundInClosed[i]))
                expandedNodes[i]->insertion_sort();
            else
            {
                delete expandedNodes[i];
                expandedNodes[i] = nullptr;
            }
        }
        current->close();
    }

    keepRunning = false;
    for (int i = 0; i < 4; i++)
        expandedNodes[i] = nullptr;

    before.wait();
    after.wait();

    for (int i = 0; i < 8; i++)
        threads[i].join();

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

    Tile::opened = new Tile(arr);
    bool solved = false;

    if (argc > 1)
    {
        switch (argv[1][0])
        {
        case '2':
            solved = solve_with_2_threads();
            break;
        case '3':
            solved = solve_with_2_threads2();
            break;
        case '8':
            solved = solve_with_n_threads();
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