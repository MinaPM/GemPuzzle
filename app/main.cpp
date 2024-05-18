#include "TileShape.h"
#include "TileControls.h"
#include <unistd.h>
#include <thread>

TileGrid tileShape;
Tile goal;
TileControls *tileControls;
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
		usleep(100000 * (tileControls->solution_speed_slider.max - tileControls->solution_speed_slider.current));
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
		if (!(tile->*moves[1][mv])())
		{
			level++;
			continue;
		}

		tileShape.update_values(*tile);
		usleep(10000);
	}
}

bool solve_puzzle()
{

	bool solvable = false;
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
		usleep(10000 * (tileControls->solving_speed_slider.max - tileControls->solving_speed_slider.current));

		// usleep(1000000);
	}
	if (!solvable)
	{
		std::cout << "No solution found\n";
	}
	return false;
}

int main()
{

	// window settings
	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(1000, 720), "Gem Puzzle", sf::Style::Close, settings);
	window.setFramerateLimit(30);

	// setting the seed for the random function
	srand(time(NULL));

	// intit goal tile
	goal = set_goal();
	Tile::opened = new Tile(goal);
	Tile::opened->update_fgh();
	tileShape.update_values(*Tile::opened);

	// loading font
	sf::Font roboto_font;
	roboto_font.loadFromFile("Assets\\Fonts\\roboto.ttf");

	tileShape.setFont(roboto_font);
	tileShape.center_tiles(window.getSize());

	tileControls = new TileControls(roboto_font);
	// tileControls.setFont(roboto_font);

	// starting solveing function
	std::thread solve;
	std::thread shuffle;

	// event for handling input

	while (true)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				return 0;
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				tileControls->mouseClicked(sf::Mouse::getPosition(window));
				if (!tileControls->start_pressed && tileControls->start.within(sf::Mouse::getPosition(window)))
				{
					tileControls->start_pressed = true;
					solve = std::thread(&solve_puzzle);
					solve.detach();
				}
				if (!tileControls->shuffle_pressed && tileControls->shuffle.within(sf::Mouse::getPosition(window)))
				{
					// tileControls->shuffle_pressed = true;
					shuffle = std::thread(&shuffle_tile, Tile::opened,
										  tileControls->shuffle_slider.current);
					shuffle.detach();
				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				tileControls->mouseReleased();
			}
			if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonPressed)
			{
				tileControls->update(sf::Mouse::getPosition(window));
			}
		}

		window.clear();
		window.draw(tileShape);
		window.draw(*tileControls);
		window.display();
	}
	solve.join();
	return 0;
}