#include "Header/PuzzleShape.h"

Puzzle *puzzlePtr = nullptr;
TileControls *tileControlsPtr = nullptr;

bool load_resourses(sf::Font &font, sf::SoundBuffer &buffer)
{
	// loading sound
	if (!buffer.loadFromFile("Assets/Audio/beep.wav"))
		return false;

	// loading font
	if (!font.loadFromFile("Assets/Fonts/roboto.ttf"))
		return false;

	return true;
}

void solve()
{
	tileControlsPtr->setFunctions(SOLVE, false);
	tileControlsPtr->setFunctions(SHUFFLE, false);
	tileControlsPtr->setFunctions(SOLUTION, false);
	tileControlsPtr->sliders[SOLVE].enable();
	tileControlsPtr->checkboxes[UseThreadsCheckBox].disable();

	puzzlePtr->solve(tileControlsPtr->checkboxes[UseThreadsCheckBox].controlable);

	if (puzzlePtr->isSolved())
		tileControlsPtr->setFunctions(SOLUTION, true);
}
void shuffle()
{
	tileControlsPtr->setFunctions(SOLVE, false);
	tileControlsPtr->setFunctions(SHUFFLE, false);
	tileControlsPtr->setFunctions(SOLUTION, false);

	puzzlePtr->shuffle();

	tileControlsPtr->setFunctions(SOLVE, true);
	tileControlsPtr->setFunctions(SHUFFLE, true);
}
void display_solution()
{
	tileControlsPtr->setFunctions(SOLVE, false);
	tileControlsPtr->setFunctions(SHUFFLE, false);
	tileControlsPtr->setFunctions(SOLUTION, false);
	tileControlsPtr->sliders[SOLUTION].enable();

	puzzlePtr->display_solution();
	tileControlsPtr->setFunctions(SOLUTION, true);
}

int main()
{

	// window settings
	sf::ContextSettings settings;
	settings.antialiasingLevel = 16;
	sf::RenderWindow window(sf::VideoMode(1000, 720), "Gem Puzzle", sf::Style::Close, settings);
	window.setFramerateLimit(30);

	sf::SoundBuffer buffer;
	sf::Font roboto_font;
	if (!load_resourses(roboto_font, buffer))
		return -1;

	TileControls tileControls(roboto_font);
	TileData tileData(roboto_font);
	TileGrid tileShape(roboto_font, buffer, tileControls.checkboxes[SoundCheckBox].controlable);

	Puzzle puzzle(tileControls, tileData, tileShape);

	tileControlsPtr = &tileControls;
	puzzlePtr = &puzzle;

	tileShape.center_tiles(window.getSize());
	tileData.setPosition(10, 450);

	tileControls.buttons[SOLVE].setOnClick(solve);
	tileControls.buttons[SOLUTION].setOnClick(display_solution);
	tileControls.buttons[SHUFFLE].setOnClick(shuffle);

	tileControlsPtr->setFunctions(SOLVE, false);
	tileControlsPtr->setFunctions(SOLUTION, false);

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
				tileControls.mouseClicked(sf::Mouse::getPosition(window));
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				tileControls.mouseReleased();
			}
			if (event.type == sf::Event::MouseMoved || event.type == sf::Event::MouseButtonPressed)
			{
				tileControls.update(sf::Mouse::getPosition(window));
			}
		}

		window.clear();
		window.draw(tileShape);
		window.draw(tileControls);
		window.draw(tileData);
		window.display();
	}
	return 0;
}