#pragma once

struct VictoryStatus
{
    bool interrupted;
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
        interrupted = false;
    }
};