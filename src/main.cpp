#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"
#include "editor/center-text-control.hpp"
#include "common/time_render.hpp"
#include <time.h>
#include <stdlib.h>
#include <thread>
using namespace edtr;

void drawPollText(sf::RenderWindow& w, sf::Text t);

int main()
{
    sf::Clock clock;
    sf::Clock poll_clock;
    sf::Clock poll_timer;
	sf::ContextSettings settings;
    sf::Font font;
    sf::Text polltxt;
    Conf::loadUserConf();
    Conf::loadTextures();
    bool showPollText = false;
	settings.antialiasingLevel = 4;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);

    while (window.isOpen())
    {
        srand(time(NULL));
        // Initialize simulation
        Simulation simulation(window);
        // simulation.loadMap(MAP_NAME);
        simulation.loadMap(Conf::chooseMap());
        // Create editor scene around it
        GUI::Scene::Ptr scene = create<edtr::EditorScene>(window, simulation);
        scene->resize();
        std::shared_ptr<TimeController> timer = scene->getByName<TimeController>("timer");
        clock.restart();
        poll_clock.restart();
        TextControl tc(*Conf::GLOBAL_FONT);
        // start simulation immediately after booting up.
        timer->current_state = TimeController::State::Play;
        timer->select(TimeController::State::Play);

        while (simulation.isRunning) // isRunning should be set to false when the win condition is implemented.
        {
            // Update
            scene->update();
            // Render
            window.clear(sf::Color(75, 75, 75));
            scene->render();

            if (clock.getElapsedTime().asMilliseconds() < 5000)
            {
                window.draw(tc.getText(simulation.vstat, true));
            }

            if (poll_clock.getElapsedTime().asSeconds() > 180 && simulation.earlyStoppingPoll == false) // every 3 minutes start the poll
            {
                simulation.cl_cont.server_Create_Poll("_ " + simulation.getCurrentColoniesStr());
                poll_clock.restart();
            }

            // Draw timer
            window.draw(tc.getTimeText(TimeRender::HumanReadableTime(clock.getElapsedTime().asMilliseconds())));
            window.display();

            // Draw poll effects
            if (simulation.pollstr != NULL)
            {
                showPollText = true;
                poll_timer.restart();
                polltxt = *simulation.pollstr;
                simulation.pollstr = NULL;
            }

            if (showPollText && poll_timer.getElapsedTime().asSeconds() < 5)
            {
                window.draw(polltxt);
            }
        }
        
        clock.restart();

        while (clock.getElapsedTime().asMilliseconds() < 5000)
        {
            // Update
            scene->update();
            // Render
            window.clear(sf::Color(100, 100, 100));
            scene->render();
            window.draw(tc.getText(simulation.vstat, false));
            window.display();
        }

        cout << "Instance ended ... Going to restart." << endl;
    }

    Conf::freeTextures();
	return 1;
}
