#include <SFML/Graphics.hpp>
#include "tile.h"

class TileShape : public sf::Drawable, public sf::Transformable
{
public:
    sf::RectangleShape tilesShapes[N][N];
    sf::Text tile_numbers[N][N];
    sf::Text tile_fgh[N][N];
    sf::Font roboto_font;
    float tile_size;
    int max_dist;

    TileShape()
    {
        max_dist = 2 * (N - 1);
        tile_size = 100;
        roboto_font.loadFromFile("Assets\\Fonts\\roboto.ttf");
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                // init shapes
                tilesShapes[i][j].setSize(sf::Vector2f(tile_size, tile_size));
                tilesShapes[i][j].setPosition(i * tile_size, j * tile_size);
                tilesShapes[i][j].setFillColor(sf::Color::White);
                tilesShapes[i][j].setOutlineColor(sf::Color::Red);
                tilesShapes[i][j].setOutlineThickness(2);
                tilesShapes[i][j].setOrigin(tile_size / 2, tile_size / 2);

                // intit tile number
                tile_numbers[i][j].setCharacterSize(30);
                tile_numbers[i][j].setFillColor(sf::Color::Black);
                tile_numbers[i][j].setStyle(sf::Text::Bold);
                tile_numbers[i][j].setFont(roboto_font);
                // init tile fgh
                tile_fgh[i][j].setCharacterSize(20);
                tile_fgh[i][j].setFillColor(sf::Color::Black);
                tile_fgh[i][j].setFont(roboto_font);
            }
        center_text();
    }

    void center_tiles(sf::Vector2u window_size)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                tilesShapes[i][j].setPosition(
                    (window_size.x / 2 - tile_size * N / 2) + j * tile_size + tile_size / 2,
                    (window_size.y / 2 - tile_size * N / 2) + i * tile_size + tile_size / 2);
            }
        center_text();
    }
    void center_text()
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                tile_numbers[i][j].setPosition(
                    tilesShapes[i][j].getPosition().x - tile_numbers[i][j].getLocalBounds().width / 2,
                    tilesShapes[i][j].getPosition().y - tile_numbers[i][j].getLocalBounds().height / 2 - 10);

                tile_fgh[i][j].setPosition(
                    tilesShapes[i][j].getPosition().x-tilesShapes[i][j].getOrigin().x,
                    tilesShapes[i][j].getPosition().y-tilesShapes[i][j].getOrigin().y);
            }
    }

    void update_values(const Tile &tile)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                tile_numbers[i][j].setString(((tile.tiles[i][j] == 0) ? "" : std::to_string(tile.tiles[i][j])));
                tile_fgh[i][j].setString(std::to_string(tile.H[i][j]));
                if (tile.tiles[i][j] != 0)
                    tilesShapes[i][j].setFillColor(sf::Color(
                        255,
                        255 - (255 * tile.H[i][j] / max_dist),
                        255 - (255 * tile.H[i][j] / max_dist)));
                else
                    tilesShapes[i][j].setFillColor(sf::Color::Yellow);
            }

        center_text();
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                rt.draw(tilesShapes[i][j], states);
                rt.draw(tile_numbers[i][j], states);
                rt.draw(tile_fgh[i][j], states);
            }
        }
    }
};
