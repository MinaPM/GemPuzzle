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

    sf::Vector2f getPosition()
    {
        return rectangle.getPosition();
    }

    sf::Vector2f getSize()
    {
        return rectangle.getSize();
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
    void (*onClick)();
    Button(const sf::Font &font, const std::string &text, int size) : Control(font, text, size)
    {
        rectangle.setSize(sf::Vector2f(lable.getLocalBounds().width + 10,
                                       lable.getLocalBounds().height + 10));
        setPosition(0, 0);
        setColor(sf::Color::White, sf::Color::Red, sf::Color::Red);
        onClick = nullptr;
    }

    void setPosition(float x, float y)
    {
        rectangle.setPosition(x, y);
        lable.setPosition(rectangle.getPosition());
    }

    void click()
    {
        if (!enabled || onClick == nullptr)
            return;
        if (button_thread.joinable())
            button_thread.join();

        button_thread = std::thread(&Button::run, this);
    }
    void setOnClick(void (*func)())
    {
        onClick = func;
    }

private:
    void run()
    {
        disable();
        if (onClick)
            onClick();
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

enum Buttons
{
    SolveButton,
    ShuffleButton,
    ShowSolutionButton
};

enum Sliders
{
    ShuffleSlider,
    SolvingSpeedSlider,
    SolutionSpeedSlider
};

class TileControls : public sf::Drawable,
                     public sf::Transformable
{

public:
    Button buttons[3];
    Slider sliders[3];
    CheckBox sound_check;
    TileControls(const sf::Font &font)
        : sliders{
              Slider(font, "Shuffle intensity", 20, 30, 4, 100),
              Slider(font, "Solving speed", 20, 5, 1, 10),
              Slider(font, "Solution speed", 20, 1, 1, 10)},
          buttons{
              Button(font, "Solve", 20),
              Button(font, "Shuffle", 20),
              Button(font, "Show Solution", 20),
          },
          sound_check(font, "Sounds", 20, true)
    {

        sound_check.setPosition(20, 5);
        int x = 20;
        for (auto &button : buttons)
        {
            button.setPosition(x, 40);
            x = button.getPosition().x + button.getSize().x + 20;
        }
        x = 100;
        for (auto &slider : sliders)
        {
            slider.setPosition(20, x);
            x += 80;
        }
    }

    void mouseClicked(sf::Vector2i point)
    {
        for (auto &slider : sliders)
            slider.clickWithin(point);

        sound_check.clickWithin(point);

        for (auto &button : buttons)
        {
            if (button.within(point))
                button.click();
        }
    }

    void enabelAllButtons()
    {
        for (auto &button : buttons)
            button.enable();
    }
    void disabelAllButtons()
    {
        for (auto &button : buttons)
            button.disable();
    }
    void mouseReleased()
    {
        for (auto &slider : sliders)
            slider.mouseReleased();
    }

    void update(sf::Vector2i point)
    {
        for (auto &slider : sliders)
            slider.setValue(point);
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        for (auto &button : buttons)
            rt.draw(button, states);

        for (auto &slider : sliders)
            rt.draw(slider, states);

        rt.draw(sound_check, states);
    }
};
