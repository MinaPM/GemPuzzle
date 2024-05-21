#include <SFML/Graphics.hpp>
#include <thread>

// template <typename Controlable>
/*class Control : public sf::Drawable, public sf::Transformable
{
    sf::RectangleShape rectangle;
    sf::Text lable;
    // Controlable controlable;
    bool enabled;

    Control(){enabled=true;};

    Control(const sf::Font &font)
    {
        Control();
        lable.setFont(font);
    }


    void setColor(sf::Color rectangleFill, sf::Color rectangleOutline, sf::Color text) {
        rectangle.setFillColor(rectangleFill);
        rectangle.setOutlineColor(rectangleOutline);
        lable.setFillColor(text);
    }

    bool within(sf::Vector2i point) { return rectangle.getGlobalBounds().contains((sf::Vector2f)point); }

    bool isEnabled() { return enabled; }
    void enable() { enabled = true; }
    void disable() { enabled = false; }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(rectangle, states);
        rt.draw(lable, states);
    }
};*/

class Button : public sf::Drawable, public sf::Transformable
{
public:
    sf::Text button_text;
    sf::RectangleShape button_rect;
    bool enabled;
    std::thread button_thread;
    void (*onclick)();
    Button(const sf::Font &font, const std::string &text, int size)
    {
        button_text.setFont(font);
        button_text.setCharacterSize(size);
        button_text.setString(text);

        enable();

        // enabled = true;
        // setColor(sf::Color::Blue, sf::Color::White, sf::Color::Blue);

        button_rect.setSize(sf::Vector2f(button_text.getLocalBounds().width + 10,
                                         button_text.getLocalBounds().height + 10));
        button_rect.setOutlineThickness(2);

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

    void disable()
    {
        enabled = false;
        setColor(sf::Color(50, 50, 50), sf::Color::White, sf::Color(50, 50, 50));
    }
    void enable()
    {
        enabled = true;
        setColor(sf::Color::Blue, sf::Color::White, sf::Color::Blue);
    }

    void setColor(sf::Color text, sf::Color fill, sf::Color outline)
    {
        button_text.setFillColor(text);
        button_rect.setFillColor(fill);
        button_rect.setOutlineColor(outline);
    }

    template <typename Func, typename... Args>
    void run(Func function, Args... args)
    {
        if (!enabled)
            return;
        if (button_thread.joinable())
            button_thread.join();

        button_thread = std::thread(&Button::runFunc<Func, Args...>, this, function, args...);
    }

    bool within(sf::Vector2i point)
    {
        return button_rect.getGlobalBounds().contains((sf::Vector2f)point);
    }

private:
    template <typename Func, typename... Args>
    void runFunc(Func function, Args... args)
    {
        disable();
        function(args...);
        enable();
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

        current_text.setFont(font);
        current_text.setCharacterSize(textSize);
        current_text.setString(std::to_string(current));

        range_rect.setOutlineThickness(3);
        range_rect.setSize(sf::Vector2f(width, 20));

        current_rect.setSize(sf::Vector2f((float)((current - min) / (float)(max - min)) * width, 20));

        setColor(sf::Color::White, sf::Color::Red, sf::Color::White);
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
        setColor(sf::Color::White, sf::Color::Red, sf::Color::White);
    }

    void disable()
    {
        enabled = false;
        setColor(sf::Color(100, 100, 100), sf::Color(100, 0, 0), sf::Color(100, 100, 100));
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

    void setColor(sf::Color fill, sf::Color outline, sf::Color text)
    {
        range_rect.setFillColor(fill);
        range_rect.setOutlineColor(outline);
        current_rect.setFillColor(outline);
        current_text.setFillColor(text);
        slider_text.setFillColor(text);
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

class CheckBox : public sf::Drawable, public sf::Transformable
{

public:
    sf::Text check_text;
    sf::RectangleShape check_rect;
    sf::CircleShape check_circle;
    bool enabled, status;
    CheckBox(const sf::Font &font, const std::string &text, int size, bool status = false)
    {
        check_text.setFont(font);
        check_text.setCharacterSize(size);
        check_text.setString(text);

        this->status = status;

        enable();

        // enabled = true;
        setColor(sf::Color::White, sf::Color::White, sf::Color::Blue);

        check_rect.setSize(sf::Vector2f(15, 15));
        check_circle.setRadius(7.5);
        check_circle.setFillColor(sf::Color::Red);
        check_rect.setOutlineThickness(2);

        setPosition(0, 0);
    }

    void setPosition(float x, float y)
    {
        check_rect.setPosition(x, y);
        check_circle.setPosition(check_rect.getPosition());
        check_text.setPosition(check_rect.getPosition().x + check_rect.getGlobalBounds().width + 5, check_rect.getPosition().y);
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(check_rect, states);
        if (status)
            rt.draw(check_circle, states);

        rt.draw(check_text, states);
    }

    void disable()
    {
        enabled = false;
        setColor(sf::Color(50, 50, 50), sf::Color::White, sf::Color(50, 50, 50));
    }
    void enable()
    {
        enabled = true;
        setColor(sf::Color::Blue, sf::Color::White, sf::Color::Blue);
    }

    void setColor(sf::Color text, sf::Color fill, sf::Color outline)
    {
        check_text.setFillColor(text);
        check_rect.setFillColor(fill);
        check_rect.setOutlineColor(outline);
    }

    const bool &bindStatus() { return status; }

    void toggle()
    {
        // check if unchecked and vice versa
        status = !status;
    }

    void ckeck()
    {
        status = true;
        // draw the circle
    }

    void unckeck()
    {
        status = false;
        // dont draw the circle
    }

    bool within(sf::Vector2i point)
    {
        return check_rect.getGlobalBounds().contains((sf::Vector2f)point);
    }
    bool clickWithin(sf::Vector2i point)
    {
        if (within(point))
        {
            toggle();
            return true;
        }
        else
            return false;
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
    CheckBox sound_check;
    TileControls(const sf::Font &font)
        : start(font, "Start", 20),
          shuffle(font, "Shuffle", 20),
          shuffle_slider(font, "Shuffle intensity", 20, 30, 4, 100),
          solving_speed_slider(font, "Solving speed", 20, 5, 1, 10),
          solution_speed_slider(font, "Solution speed", 20, 1, 1, 10),
          sound_check(font, "Sounds", 20, true)
    {
        start_pressed = 0;
        shuffle_pressed = 0;
        isMouseClicked = 0;
        sound_check.setPosition(20, 5);
        shuffle.setPosition(20, 40);
        start.setPosition(shuffle.getPosition().x + 100, 40);
        shuffle_slider.setPosition(20, 100);
        solving_speed_slider.setPosition(20, 180);
        solution_speed_slider.setPosition(20, 260);
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
        sound_check.clickWithin(point);
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
        rt.draw(sound_check, states);
    }
};
