#include <SFML/Graphics.hpp>

class Button : public sf::Drawable, public sf::Transformable
{

public:
    sf::Text button_text;
    sf::RectangleShape button_rect;
    bool enabled;
    void (*onclick)();
    Button(const sf::Font &font, const std::string &text, int size)
    {
        enabled=true;
        button_text.setFont(font);
        button_text.setCharacterSize(size);
        button_text.setString(text);
        button_text.setFillColor(sf::Color::Blue);

        button_rect.setFillColor(sf::Color::White);
        button_rect.setOutlineColor(sf::Color::Blue);
        button_rect.setOutlineThickness(2);

        button_rect.setSize(sf::Vector2f(button_text.getLocalBounds().width + 10,
                                         button_text.getLocalBounds().height + 10));

        setPosition(0, 0);
    }

    void setPosition(float x, float y)
    {
        button_rect.setPosition(x, y);
        button_text.setPosition(button_rect.getPosition());
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(button_rect, states);
        rt.draw(button_text, states);
    }

    void setOnclick(void (*func)())
    {
        onclick = func;
    }
    bool within(sf::Vector2i point)
    {
        return button_rect.getGlobalBounds().contains((sf::Vector2f)point);
    }
};

class Slider : public sf::Drawable, public sf::Transformable
{

public:
    sf::Text slider_text, current_text;
    sf::RectangleShape range_rect, current_rect;
    int min, max, current, width;
    bool enabled;

    Slider(const sf::Font &font, const std::string &text, int textSize, int current, int min, int max)
    {
        enabled = true;
        width = 100;
        this->min = min;
        this->max = max;
        this->current = current;
        slider_text.setFont(font);
        slider_text.setCharacterSize(textSize);
        slider_text.setString(text);
        slider_text.setFillColor(sf::Color::White);

        current_text.setFont(font);
        current_text.setCharacterSize(textSize);
        current_text.setString(std::to_string(current));
        current_text.setFillColor(sf::Color::White);

        range_rect.setFillColor(sf::Color::White);
        range_rect.setSize(sf::Vector2f(width, 20));

        range_rect.setFillColor(sf::Color::Red);
        current_rect.setSize(sf::Vector2f((float)((current - min) / (float)(max - min)) * width, 20));

        setPosition(0, 0);
    }

    void setPosition(float x, float y)
    {
        slider_text.setPosition(x, y);
        range_rect.setPosition(x, y + slider_text.getCharacterSize() + 5);
        current_rect.setPosition(range_rect.getPosition());
        current_text.setPosition(range_rect.getPosition().x + range_rect.getGlobalBounds().width + 5, range_rect.getPosition().y);
    }

    bool within(sf::Vector2i point)
    {
        return range_rect.getGlobalBounds().contains((sf::Vector2f)point);
    }

    void enable()
    {
        enabled = true;
        //range_rect.setFillColor(sf::Color::White);
        range_rect.setFillColor(sf::Color::Red);
    }

    void disable()
    {
        enabled = false;
       // range_rect.setFillColor(sf::Color::White);
        range_rect.setFillColor(sf::Color(50, 50, 50));
    }

    void setValue(sf::Vector2i point)
    {
        if (!enabled)
            return;
        current_rect.setSize(sf::Vector2f(
            std::min(
                width, point.x - (int)current_rect.getGlobalBounds().left),
            20));
        current = ((float)std::min(width, point.x - (int)current_rect.getGlobalBounds().left) / width) * (max - min) + min;
        current_text.setString(std::to_string(current));
    }
    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(slider_text, states);
        rt.draw(range_rect, states);
        rt.draw(current_rect, states);
        rt.draw(current_text, states);
    }
};

class TileControls : public sf::Drawable,
                     public sf::Transformable
{
public:
    Button start;
    bool start_pressed;
    Button shuffle;
    bool shuffle_pressed;
    Slider shuffle_slider;
    TileControls(const sf::Font &font) : start(font, "Start", 20),
                                         shuffle(font, "Shuffle", 20),
                                         shuffle_slider(font, "Shuffle intensity", 20, 30, 5, 60)
    {
        start_pressed = 0;
        shuffle_pressed = 0;
        shuffle.setPosition(20, 5);
        start.setPosition(20, 40);
        shuffle_slider.setPosition(500, 5);
    }

    void setFont(const sf::Font &font)
    {
        return;
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(start, states);
        rt.draw(shuffle, states);
        rt.draw(shuffle_slider, states);
    }
};
