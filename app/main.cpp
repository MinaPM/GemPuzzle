#include "TileShape.h"
// #include <chrono>
#include <unistd.h>
#include <thread>

TileShape tileShape;
Tile goal;

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

void display_list_reversed(Tile *node)
{
	if (node == nullptr)
	{
		return;
	}
	else
	{
		display_list_reversed(node->previous);
		tileShape.update_values(*node);
		sleep(1);
	}
}

void shuffle_tile(Tile *tile, int level)
{
	if (tile == nullptr || level < 1)
		return;

	int mv;
	while (level--)
	{
		mv = rand() % 4;
		if(!(tile->*moves[1][mv])())
		level++;
	}
}

bool solve_puzzle()
{

	bool solvable = false;
	shuffle_tile(Tile::opened, 100);

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
			display_list_reversed(current);
			return true;
		}
		expand(current);
		current->close();
		// usleep();
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

	srand(time(NULL));
	goal = set_goal();
	Tile::opened = new Tile(goal);
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