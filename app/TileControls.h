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
        enabled = true;
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
    bool mouseClicked;

    Slider(const sf::Font &font, const std::string &text, int textSize, int current, int min, int max)
    {
        enabled = true;
        width = 100;
        this->min = min;
        this->max = max;
        this->current = current;
        mouseClicked = 0;
        slider_text.setFont(font);
        slider_text.setCharacterSize(textSize);
        slider_text.setString(text);
        slider_text.setFillColor(sf::Color::White);

        current_text.setFont(font);
        current_text.setCharacterSize(textSize);
        current_text.setString(std::to_string(current));
        current_text.setFillColor(sf::Color::White);

        range_rect.setFillColor(sf::Color::White);
        range_rect.setOutlineColor(sf::Color::Red);
        range_rect.setOutlineThickness(3);
        range_rect.setSize(sf::Vector2f(width, 20));

        current_rect.setFillColor(sf::Color::Red);
        current_rect.setSize(sf::Vector2f((float)((current - min) / (float)(max - min)) * width, 20));

        setPosition(0, 0);
    }

    void setPosition(float x, float y)
    {
        slider_text.setPosition(x, y);
        range_rect.setPosition(x, y + slider_text.getCharacterSize() + 10);
        current_rect.setPosition(range_rect.getPosition());
        current_text.setPosition(range_rect.getPosition().x + range_rect.getGlobalBounds().width + 5, range_rect.getPosition().y);
    }

    bool clickWithin(sf::Vector2i point)
    {
        if (range_rect.getGlobalBounds().contains((sf::Vector2f)point))
        {
            mouseClicked = true;
            return true;
        }
        else
            return false;
    }

    void enable()
    {
        enabled = true;
        // range_rect.setFillColor(sf::Color::White);
        range_rect.setFillColor(sf::Color::Red);
    }

    void disable()
    {
        enabled = false;
        range_rect.setFillColor(sf::Color(50, 50, 50));
    }

    void setValue(sf::Vector2i point)
    {
        if (!enabled || !mouseClicked)
            return;
        float lenght = std::max(0.f, std::min((float)width, point.x - current_rect.getGlobalBounds().left));

        current_rect.setSize(sf::Vector2f(lenght, current_rect.getSize().y));
        current = (lenght / width) * (max - min) + min;
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
    bool isMouseClicked;
    Slider shuffle_slider;
    Slider solving_speed_slider;
    Slider solution_speed_slider;
    TileControls(const sf::Font &font)
        : start(font, "Start", 20),
          shuffle(font, "Shuffle", 20),
          shuffle_slider(font, "Shuffle intensity", 20, 30, 4, 100),
          solving_speed_slider(font, "Solving speed", 20, 3, 1, 10),
          solution_speed_slider(font, "Solution speed", 20, 1, 1, 10)
    {
        start_pressed = 0;
        shuffle_pressed = 0;
        isMouseClicked = 0;
        shuffle.setPosition(20, 5);
        start.setPosition(20, 40);
        shuffle_slider.setPosition(20, 100);
        solving_speed_slider.setPosition(20, 200);
        solution_speed_slider.setPosition(20, 300);
    }

    void setFont(const sf::Font &font)
    {
        return;
    }

    void mouseClicked(sf::Vector2i point)
    {
        isMouseClicked = true;
        shuffle_slider.clickWithin(point);
        solving_speed_slider.clickWithin(point);
        solution_speed_slider.clickWithin(point);
    }
    void mouseReleased()
    {
        isMouseClicked = false;
        shuffle_slider.mouseClicked = false;
        solving_speed_slider.mouseClicked = false;
        solution_speed_slider.mouseClicked = false;
    }

    void update(sf::Vector2i point)
    {
        shuffle_slider.setValue(point);
        solving_speed_slider.setValue(point);
        solution_speed_slider.setValue(point);
    }
    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(start, states);
        rt.draw(shuffle, states);
        rt.draw(shuffle_slider, states);
        rt.draw(solving_speed_slider, states);
        rt.draw(solution_speed_slider, states);
    }
};
