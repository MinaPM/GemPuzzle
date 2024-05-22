#include <SFML/Graphics.hpp>
#include <thread>

// template <typename Controlable>
class Control : public sf::Drawable, public sf::Transformable
{
public:
    sf::RectangleShape rectangle;
    sf::Text lable;
    // Controlable controlable;
    bool enabled, clicked;
    Control()
    {
        enabled = true;
        clicked = false;
        lable.setCharacterSize(20);
        rectangle.setOutlineThickness(3);
    };
    Control(const sf::Font &font) : Control()
    {
        lable.setFont(font);
    }
    Control(const sf::Font &font, const std::string &string, int characterSize = 20) : Control(font)
    {
        lable.setString(string);
        lable.setCharacterSize(characterSize);
    }

    void setFont(const sf::Font &font) { lable.setFont(font); }
    void setString(const std::string &string) { lable.setString(string); }
    void setColor(sf::Color rectangleFill, sf::Color rectangleOutline, sf::Color text)
    {
        rectangle.setFillColor(rectangleFill);
        rectangle.setOutlineColor(rectangleOutline);
        lable.setFillColor(text);
    }

    bool within(sf::Vector2i point) { return rectangle.getGlobalBounds().contains((sf::Vector2f)point); }
    bool clickWithin(sf::Vector2i point) { return clicked = within(point); }
    void mouseReleased() { clicked = false; }

    bool isEnabled() { return enabled; }
    void enable()
    {
        enabled = true;
        sf::Color c1(rectangle.getFillColor()), c2(rectangle.getOutlineColor()), c3(lable.getFillColor());
        c1.a = 255;
        c2.a = 255;
        c3.a = 255;
        setColor(c1, c2, c3);
    }
    void disable()
    {
        enabled = false;
        sf::Color c1(rectangle.getFillColor()), c2(rectangle.getOutlineColor()), c3(lable.getFillColor());
        c1.a = 124;
        c2.a = c1.a;
        c3.a = c1.a;
        setColor(c1, c2, c3);
    }

    void alignLeft()
    {
        lable.setPosition(rectangle.getPosition().x, lable.getPosition().y);
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        rt.draw(rectangle, states);
        rt.draw(lable, states);
    }
};

class Button : public Control
{
public:
    std::thread button_thread;
    void (*onclick)();
    Button(const sf::Font &font, const std::string &text, int size) : Control(font, text, size)
    {
        rectangle.setSize(sf::Vector2f(lable.getLocalBounds().width + 10,
                                       lable.getLocalBounds().height + 10));
        setPosition(0, 0);
        setColor(sf::Color::White, sf::Color::Red, sf::Color::Red);
    }

    void setPosition(float x, float y)
    {
        rectangle.setPosition(x, y);
        lable.setPosition(rectangle.getPosition());
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

private:
    template <typename Func, typename... Args>
    void runFunc(Func function, Args... args)
    {
        disable();
        function(args...);
        enable();
    }
};

class Slider : public Control
{

public:
    sf::Text current_text;
    sf::RectangleShape current_rect;
    int min, max, current, width;

    Slider(const sf::Font &font, const std::string &text, int size, int current, int min, int max) : Control(font, text, size)
    {
        width = 100;
        this->min = min;
        this->max = max;
        this->current = current;
        current_text = sf::Text(lable);
        current_text.setString(std::to_string(current));

        rectangle.setSize(sf::Vector2f(width, 20));
        current_rect.setSize(sf::Vector2f((float)((current - min) / (float)(max - min)) * width, 20));

        setColor(sf::Color::White, sf::Color::Red, sf::Color::White);
        setPosition(0, 0);
    }

    void setPosition(float x, float y)
    {
        lable.setPosition(x, y);
        rectangle.setPosition(x, y + lable.getCharacterSize() + 10);
        current_rect.setPosition(rectangle.getPosition());
        current_text.setPosition(rectangle.getPosition().x + width + 5, rectangle.getPosition().y);
    }

    void setFont(const sf::Font &font)
    {
        Control::setFont(font);
        current_text.setFont(font);
    }

    void setValue(sf::Vector2i point)
    {
        if (!enabled || !clicked)
            return;

        float lenght = std::max(0.f, std::min((float)width, point.x - current_rect.getGlobalBounds().left));
        current_rect.setSize(sf::Vector2f(lenght, current_rect.getSize().y));
        current = (lenght / width) * (max - min) + min;
        current_text.setString(std::to_string(current));
    }

    void enable()
    {
        Control::enable();
        setColor(rectangle.getFillColor(), rectangle.getOutlineColor(), lable.getFillColor());
    }
    void disable()
    {
        Control::disable();
        setColor(rectangle.getFillColor(), rectangle.getOutlineColor(), lable.getFillColor());
    }

    void setColor(sf::Color fill, sf::Color outline, sf::Color text)
    {
        Control::setColor(fill, outline, text);
        current_rect.setFillColor(outline);
        current_text.setFillColor(text);
    }
    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        Control::draw(rt, states);
        states.transform *= getTransform();
        rt.draw(current_rect, states);
        rt.draw(current_text, states);
    }
};

class CheckBox : public Control
{

public:
    sf::CircleShape check_circle;
    bool status;
    CheckBox(const sf::Font &font, const std::string &text, int size, bool status = false) : Control(font, text, size)
    {
        this->status = status;
        rectangle.setSize(sf::Vector2f(15, 15));
        check_circle.setRadius(rectangle.getSize().x / 2);
        check_circle.setFillColor(sf::Color::Red);
        setPosition(0, 0);
    }

    void setPosition(float x, float y)
    {
        rectangle.setPosition(x, y);
        check_circle.setPosition(rectangle.getPosition());
        lable.setPosition(rectangle.getPosition().x + rectangle.getGlobalBounds().width + 5,
                          rectangle.getPosition().y - lable.getGlobalBounds().height / 2);
    }

    void enable()
    {
        enabled = true;
        setColor(sf::Color::Blue, sf::Color::White, sf::Color::Blue);
    }
    void disable()
    {
        enabled = false;
        setColor(sf::Color(50, 50, 50), sf::Color::White, sf::Color(50, 50, 50));
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        Control::draw(rt, states);
        states.transform *= getTransform();
        if (status)
            rt.draw(check_circle, states);
    }

    void setColor(sf::Color fill, sf::Color outline, sf::Color text)
    {
        Control::setColor(fill, outline, text);
        check_circle.setFillColor(outline);
    }

    const bool &bindStatus() { return status; }

    void toggle() { status = !status; }

    void ckeck() { status = true; }

    void unckeck() { status = false; }

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
    Button shuffle;
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

        sound_check.setPosition(20, 5);
        shuffle.setPosition(20, 40);
        start.setPosition(shuffle.getPosition().x + 100, 40);
        shuffle_slider.setPosition(20, 100);
        solving_speed_slider.setPosition(20, 180);
        solution_speed_slider.setPosition(20, 260);
    }

    void mouseClicked(sf::Vector2i point)
    {
        shuffle_slider.clickWithin(point);
        solving_speed_slider.clickWithin(point);
        solution_speed_slider.clickWithin(point);
        sound_check.clickWithin(point);
    }
    void mouseReleased()
    {
        shuffle_slider.mouseReleased();
        solving_speed_slider.mouseReleased();
        solution_speed_slider.mouseReleased();
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
