#pragma once
#include <SFML/Graphics.hpp>
#include "simulation/config.hpp"
#include "simulation/victorystatus.hpp"

struct TextControl
{   
    sf::Font myFont;
    VictoryStatus vstat;

    explicit TextControl() {}

    explicit
    TextControl(sf::Font font)
    {
        myFont = font;
    }

    sf::Text getText(VictoryStatus& vstat, bool starting)
    {
        if (starting)
        {
            return getStartText();
        }
           
        this->vstat = vstat;

        if (vstat.interrupted)
        {
            return getRestartText();
        }
        else
        {
            return getResetText();
        }

        std::cout << "Problem in text control" << std::endl;
        return getResetText();
    }

    sf::Font getFont()
    {
        return myFont;
    }

    sf::Text *getPollText(char cmd, string color)
    {
        unordered_map<char, string> map;
        map['S'] = "Spawn";
        map['Q'] = "Quick";
        map['K'] = "Kill";
        map['F'] = "Food";
        map['M'] = "Spawn rate";

        sf::Text text("Ability " + map[cmd] + " applied to " + color + " colony.", myFont, 24);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, 40)); // Just below the timer.
        text.setColor(sf::Color::Green);

        return &text;
    }

    sf::Text getDefaultText()
    {
        return sf::Text text("", myFont, 1);
    }

    sf::Text getStartText()
    {
        sf::Text text("Simulation started! Enter commands in chat and coordinate with each other to help a colony win!", myFont, 32);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, Conf::WIN_HEIGHT / 2.0f));
        text.setColor(sf::Color::Red);

        return text;
    }

    sf::Text getResetText()
    {
        sf::Text text("The winner was the " + vstat.winner + " colony!", myFont, 50);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, Conf::WIN_HEIGHT / 2.0f));
        text.setColor(sf::Color::Red);

        return text;
    }

    sf::Text getRestartText()
    {
        sf::Text text("Restarting ...", myFont, 50);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, Conf::WIN_HEIGHT / 2.0f));
        text.setColor(sf::Color::Red);

        return text;
    }

    sf::Text getTimeText(std::string time)
    {
        sf::Text text(time, myFont, 50);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, 20));
        text.setColor(sf::Color::Red);

        return text;
    }

};