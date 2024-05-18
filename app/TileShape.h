#include <SFML/Graphics.hpp>
#include "tile.h"

class TileShape : public sf::Drawable, public sf::Transformable
{
    sf::RectangleShape square;
    sf::Text tile_numbers;
    sf::Text tile_fgh;

public:
    float tile_size;

    TileShape()
    {
        tile_size = 100;
        // init shapes
        square.setSize(sf::Vector2f(tile_size, tile_size));
        square.setFillColor(sf::Color::White);
        square.setOutlineColor(sf::Color::Red);
        square.setOutlineThickness(2);
        square.setOrigin(tile_size / 2, tile_size / 2);

        // intit tile number
        tile_numbers.setCharacterSize(30);
        tile_numbers.setFillColor(sf::Color::Black);
        tile_numbers.setStyle(sf::Text::Bold);

        //  init tile fgh
        tile_fgh.setCharacterSize(20);
        tile_fgh.setFillColor(sf::Color::Black);
    }

    TileShape(const sf::Font &font)
    {
        TileShape();
        set_font(font);
    }

    void center_text()
    {
        tile_numbers.setPosition(
            square.getPosition().x - tile_numbers.getLocalBounds().width / 2,
            square.getPosition().y - tile_numbers.getLocalBounds().height / 2 - 10);

        tile_fgh.setPosition(
            square.getPosition().x - square.getOrigin().x,
            square.getPosition().y - square.getOrigin().y);
    }

    void set_font(const sf::Font &font)
    {
        tile_numbers.setFont(font);
        tile_fgh.setFont(font);
        center_text();
    }

    void update_values(const int &tileNumber, const int &fghValue, int max_dist)
    {

        tile_numbers.setString(((tileNumber == 0) ? "" : std::to_string(tileNumber)));
        tile_fgh.setString(std::to_string(fghValue));
        if (tileNumber != 0)
            square.setFillColor(sf::Color(
                255,
                255 - (255 * fghValue / max_dist),
                255 - (255 * fghValue / max_dist)));
        else
            square.setFillColor(sf::Color::Yellow);

        center_text();
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(square, states);
        rt.draw(tile_numbers, states);
        rt.draw(tile_fgh, states);
    }
};

class TileGrid : public sf::Drawable,
                 public sf::Transformable
{
public:
    TileShape tileShapes[N][N];

    int max_dist;

    TileGrid(/*const sf::Font &font*/)
    {
        max_dist = 2 * (N - 1);
    }

    TileGrid(const sf::Font &font)
    {
        TileGrid();
        setFont(font);
    }

    void setFont(const sf::Font &font)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                tileShapes[i][j].set_font(font);
    }

    void center_tiles(sf::Vector2u window_size)
    {
        int tileSize = tileShapes[0][0].tile_size;
        int x1 = (window_size.x - (tileSize * (N - 1))) / 2;
        int y2 = (window_size.y - (tileSize * (N - 1))) / 2;

        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                tileShapes[i][j].setPosition(x1 + j * tileSize, y2 + i * tileSize);
    }

    void update_values(const Tile &tile)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
            {
                tileShapes[i][j].update_values(
                    tile.tiles[i][j],
                    tile.H[i][j],
                    max_dist);
            }
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                rt.draw(tileShapes[i][j], states);
    }
};
