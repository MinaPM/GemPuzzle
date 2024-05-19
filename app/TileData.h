#include <SFML/Graphics.hpp>

class TileData : public sf::Drawable,
                 public sf::Transformable
{
public:
    sf::Text total_opened_text, total_closed_text, current_f_text, current_g_text, current_h_text;
    sf::Text total_opened_value_text, total_closed_value_text, current_f_value_text, current_g_value_text, current_h_value_text;

    TileData(const sf::Font &font)
    {
        total_opened_text.setFont(font);
        total_opened_text.setCharacterSize(20);
        total_opened_text.setFillColor(sf::Color::White);

        /*      */ total_closed_text = sf::Text(total_opened_text);
        /*         */ current_f_text = sf::Text(total_opened_text);
        /*         */ current_g_text = sf::Text(total_opened_text);
        /*         */ current_h_text = sf::Text(total_opened_text);
        /**/ total_opened_value_text = sf::Text(total_opened_text);

        total_opened_value_text.setStyle(sf::Text::Bold);
        /**/ total_closed_value_text = sf::Text(total_opened_value_text);
        /*   */ current_f_value_text = sf::Text(total_opened_value_text);
        /*   */ current_g_value_text = sf::Text(total_opened_value_text);
        /*   */ current_h_value_text = sf::Text(total_opened_value_text);

        total_opened_text.setString("Total Opened ");
        total_closed_text.setString("Total Closed ");
        current_f_text.setString("Current F ");
        current_g_text.setString("Current G ");
        current_h_text.setString("Current H ");
    }

    void setPosition(float x, float y)
    {
        /*      */ total_opened_text.setPosition(x, y);
        /*      */ total_closed_text.setPosition(rightPoint(total_opened_text) - total_closed_text.getGlobalBounds().width, bottomPoint(total_opened_text) + 10);
        /*         */ current_f_text.setPosition(rightPoint(total_opened_text) - current_f_text.getGlobalBounds().width, bottomPoint(total_closed_text) + 10);
        /*         */ current_g_text.setPosition(rightPoint(total_opened_text) - current_g_text.getGlobalBounds().width, bottomPoint(current_f_text) + 10);
        /*         */ current_h_text.setPosition(rightPoint(total_opened_text) - current_h_text.getGlobalBounds().width, bottomPoint(current_g_text) + 10);

        /**/ total_opened_value_text.setPosition(rightPoint(total_opened_text) + 10, /* */ total_opened_text.getPosition().y);
        /**/ total_closed_value_text.setPosition(rightPoint(total_opened_text) + 10, /**/ total_closed_text.getPosition().y);
        /*   */ current_f_value_text.setPosition(rightPoint(total_opened_text) + 10, /*   */ current_f_text.getPosition().y);
        /*   */ current_g_value_text.setPosition(rightPoint(total_opened_text) + 10, /*   */ current_g_text.getPosition().y);
        /*   */ current_h_value_text.setPosition(rightPoint(total_opened_text) + 10, /*   */ current_h_text.getPosition().y);
    }

    float rightPoint(sf::Text &t) { return t.getGlobalBounds().left + t.getGlobalBounds().width; }
    float bottomPoint(sf::Text &t) { return t.getGlobalBounds().top + t.getGlobalBounds().height; }

    void updateData(int totalOpend, int totalClosed, int f, int g, int h)
    {
        total_opened_value_text.setString(std::to_string(totalOpend));
        total_closed_value_text.setString(std::to_string(totalClosed));
        current_f_value_text.setString(std::to_string(f));
        current_g_value_text.setString(std::to_string(g));
        current_h_value_text.setString(std::to_string(h));
    }
    void draw(sf::RenderTarget &rt, sf::RenderStates states) const override
    {
        states.transform *= getTransform();

        rt.draw(total_opened_text, states);
        rt.draw(total_closed_text, states);
        rt.draw(current_f_text, states);
        rt.draw(current_g_text, states);
        rt.draw(current_h_text, states);

        rt.draw(total_opened_value_text, states);
        rt.draw(total_closed_value_text, states);
        rt.draw(current_f_value_text, states);
        rt.draw(current_g_value_text, states);
        rt.draw(current_h_value_text, states);
    }
};

/*
      total_closed_text
         current_f_text
         current_g_text
         current_h_text
total_opened_value_text
total_closed_value_text
   current_f_value_text
   current_g_value_text
   current_h_value_text
*/