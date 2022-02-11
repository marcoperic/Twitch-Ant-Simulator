#pragma once

struct VictoryStatus
{
    std::string winner;
    sf::Color color;

    explicit
    VictoryStatus()
    {
        clear();
    }

    void clear()
    {
        winner = "";
        color = sf::Color::Black;
    }
};