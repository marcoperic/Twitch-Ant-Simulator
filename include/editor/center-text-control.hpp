#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include "simulation/config.hpp"

struct TextControl
{   
    sf::Text getStartText(sf::Font& myFont)
    {
        sf::Text text("Ant simulation started! Choose a colony and help them dominate!", myFont, 50);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, Conf::WIN_HEIGHT / 2.0f));
        text.setColor(sf::Color::Red);

        return text;
    }

    sf::Text getResetText(std::string winner, sf::Font& myFont)
    {
        sf::Text text("The winner was the " + winner + " colony!", myFont, 50);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, Conf::WIN_HEIGHT / 2.0f));
        text.setColor(sf::Color::Red);

        return text;
    }

};