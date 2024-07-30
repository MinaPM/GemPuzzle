#include "Header/PuzzleShape.h"
#include <chrono>

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

	TileGrid tileShape;
	TileControls tileControls(roboto_font);
	TileData tileData(roboto_font);

	tileShape.setFont(roboto_font);
	tileShape.center_tiles(window.getSize());
	// tileShape.setSoundBuffer(buffer);

	tileData.setPosition(20, 500);
	Puzzle puzzle(tileControls, tileData, tileShape);

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
				if (tileControls.start.within(sf::Mouse::getPosition(window)))
				{
					tileControls.shuffle.disable();

					tileControls.start.run([&puzzle]()
										   { puzzle.solve(); });
				}
				if (tileControls.shuffle.within(sf::Mouse::getPosition(window)))
				{
					tileControls.start.disable();
					tileControls.shuffle_slider.disable();
					tileControls.shuffle.run([&puzzle]()
											 { puzzle.shuffle(); });
				}
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