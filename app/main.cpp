#include "TileShape.h"
#include <unistd.h>
#include <thread>

TileShape tileShape;

/***
 * Expands the node and adds it to the opened list
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

bool solve_puzzle()
{

	bool solvable = false;
	Tile goal = set_goal();

	Tile *current;

	while (Tile::opened != NULL)
	{
		current = Tile::opened;
		Tile::opened = Tile::opened->next;
		tileShape.update_values(*current);
		if (*current == goal)
		{
			int pathlength = 0;
			solvable = true;
			return true;
		}
		expand(current);
		current->close();
		usleep(50000);
	}
	if (!solvable)
	{
		std::cout << "No solution found\n";
	}
	return false;
}

int main()
{

	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(1000, 720), "Gem Puzzle", sf::Style::Close, settings);


	int arr[N][N] = {{5, 6, 1, 3},
					 {2, 0, 8, 4},
					 {9, 10, 7, 15},
					 {13, 14, 12, 11}};
	Tile::opened = new Tile(arr);
	Tile::opened->update_fgh();
	
	std::thread solve = std::thread(&solve_puzzle);
	solve.detach();

	tileShape.center_tiles(window.getSize());

	sf::Event event;
	while (true)
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				return 0;
			}
		}

		window.clear();
		window.draw(tileShape);
		window.display();
	}
	solve.join();
	return 0;
}