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
	tileControlsPtr->disabelAllButtons();

	puzzlePtr->solve();

	// tileControlsPtr->enabelAllButtons();
	if (puzzlePtr->isSolved())
		tileControlsPtr->buttons[ShowSolutionButton].enable();
}
void shuffle()
{
	tileControlsPtr->sliders[ShuffleSlider].disable();
	tileControlsPtr->disabelAllButtons();

	puzzlePtr->shuffle();

	tileControlsPtr->sliders[ShuffleSlider].enable();
	tileControlsPtr->enabelAllButtons();
	tileControlsPtr->buttons[ShowSolutionButton].disable();
}
void display_solution()
{
	tileControlsPtr->disabelAllButtons();
	puzzlePtr->display_solution();
	tileControlsPtr->buttons[ShowSolutionButton].enable();
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
	TileGrid tileShape(roboto_font, buffer, tileControls.sound_check.controlable);

	Puzzle puzzle(tileControls, tileData, tileShape);

	tileControlsPtr = &tileControls;
	puzzlePtr = &puzzle;

	tileShape.center_tiles(window.getSize());
	tileData.setPosition(20, 500);

	tileControls.buttons[SolveButton].setOnClick(solve);
	tileControls.buttons[ShowSolutionButton].setOnClick(display_solution);
	tileControls.buttons[ShuffleButton].setOnClick(shuffle);

	tileControls.buttons[SolveButton].disable();
	tileControls.buttons[ShowSolutionButton].disable();

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