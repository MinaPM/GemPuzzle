#include <SFML/Graphics.hpp>

enum Label
{
    TotalOpened,
    TotalClosed,
    CurrentF,
    CurrentG,
    CurrentH,
    Clock
};
enum Value
{
    TotalOpenedValue,
    TotalClosedValue,
    CurrentFValue,
    CurrentGValue,
    CurrentHValue,
    ClockValue
};

// const Label Labels[] = {TotalOpened, TotalClosed, CurrentF, CurrentG, CurrentH, Clock};

class TileData : public sf::Drawable,
                 public sf::Transformable
{
public:
    sf::Text labels[6], values[6];
    sf::Clock clock;

    TileData(const sf::Font &font)
    {
        for (auto &label : labels)
        {
            label.setFont(font);
            label.setCharacterSize(20);
            label.setFillColor(sf::Color::White);
        }
        for (auto &value : values)
        {
            value.setStyle(sf::Text::Bold);
            value.setFont(font);
            value.setCharacterSize(20);
            value.setFillColor(sf::Color::White);
        }

        /**/ labels[TotalOpened].setString("Total Opened ");
        /**/ labels[TotalClosed].setString("Total Closed ");
        /*   */ labels[CurrentF].setString("Current F ");
        /*   */ labels[CurrentG].setString("Current G ");
        /*   */ labels[CurrentH].setString("Current H ");
        /*      */ labels[Clock].setString("Time Elapsed (s) ");
    }

    void setPosition(float x, float y)
    {
        sf::Text &longestLabel = *getLongestLabel();

        longestLabel.setPosition(x, y);
        int rp = rightPoint(longestLabel);
        int bp = bottomPoint(longestLabel);

        labels[TotalOpened].setPosition(rp - labels[TotalOpened].getGlobalBounds().width, bp + 10);

        rp = rightPoint(labels[TotalOpened]);
        bp = bottomPoint(labels[TotalOpened]);

        for (auto &label : labels)
        {
            if (&label == &labels[TotalOpened] || &label == &longestLabel)
                continue;
            label.setPosition(rp - label.getGlobalBounds().width, bp + 10);
            bp = bottomPoint(label);
        }

        longestLabel.setPosition(longestLabel.getPosition().x, bp + 10);

        for (size_t i = 0; i < sizeof(values)/sizeof(*values); i++)
            values[i].setPosition(rp + 10, labels[i].getPosition().y);
    }

    float rightPoint(sf::Text &t) { return t.getGlobalBounds().left + t.getGlobalBounds().width; }
    float bottomPoint(sf::Text &t) { return t.getGlobalBounds().top + t.getGlobalBounds().height; }

    sf::Text *getLongestLabel()
    {
        sf::Text *longestLabel = &labels[0];
        for (auto &label : labels)
            if (label.getGlobalBounds().width >= longestLabel->getGlobalBounds().width)
                longestLabel = &label;

        return longestLabel;
    }

    void updateData(int totalOpend = 0, int totalClosed = 0, int f = 0, int g = 0, int h = 0)
    {
        /**/ values[TotalOpenedValue].setString(std::to_string(totalOpend));
        /**/ values[TotalClosedValue].setString(std::to_string(totalClosed));
        /*   */ values[CurrentFValue].setString(std::to_string(f));
        /*   */ values[CurrentGValue].setString(std::to_string(g));
        /*   */ values[CurrentHValue].setString(std::to_string(h));
        /*      */ values[ClockValue].setString(std::to_string((int)clock.getElapsedTime().asSeconds()));
    }

    void resetClock()
    {
        clock.restart();
    }

    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        for (auto &label : labels)
            rt.draw(label, states);

        for (auto &value : values)
            rt.draw(value, states);
    }
};