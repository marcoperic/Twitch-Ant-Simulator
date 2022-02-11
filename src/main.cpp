#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"

#define MAP_NAME "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\map.png"

using namespace edtr;
using namespace std;

int main()
{
    // Load configuration
    if (Conf::loadUserConf()) {
        std::cout << "Configuration file loaded." << std::endl;
    } else {
        std::cout << "Configuration file couldn't be found." << std::endl;
    }

    sf::Clock clock;
    sf::Font myFont;
    myFont.loadFromFile("res/font.ttf");
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);

    // center text
    sf::Text text("Ant simulation started! Choose a colony and help them dominate!", myFont, 50);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    text.setPosition(sf::Vector2f(Conf::WIN_WIDTH / 2.0f, Conf::WIN_HEIGHT / 2.0f));
    text.setColor(sf::Color::Red);
    
    while (window.isOpen())
    {
        // Initialize simulation
        Simulation simulation(window);
        simulation.loadMap(MAP_NAME);
        // Create editor scene around it
        GUI::Scene::Ptr scene = create<edtr::EditorScene>(window, simulation);
        scene->resize();
        std::shared_ptr<TimeController> timer = scene->getByName<TimeController>("timer");
        clock.restart();

        // start simulation immediately after booting up.
        timer->current_state = TimeController::State::Play;
        timer->select(TimeController::State::Play);

        while (simulation.isRunning) // isRunning should be set to false when the win condition is implemented.
        {
            // Update
            scene->update();
            // Render
            window.clear(sf::Color(100, 100, 100));
            scene->render();

            if (clock.getElapsedTime().asMilliseconds() < 5000)
            {
                window.draw(text);
            }

            window.display();
        }

        text.setString("the winner was the " + simulation.vstat.winner + " colony!");
        clock.restart();
        while (clock.getElapsedTime().asMilliseconds() < 5000)
        {
            // Update
            scene->update();
            // Render
            window.clear(sf::Color(100, 100, 100));
            scene->render();
            window.draw(text);
            window.display();
        }

        cout << "Instance ended ... Going to restart." << endl;
        // Display text here and countdown between maps.
        // At this point, map should be generated or selected from pool.
    }

    // LEGACY GAME LOOP
    
    // // Initialize simulation
    // Simulation simulation(window);
    // simulation.loadMap(MAP_NAME);
    // // Create editor scene around it
    // GUI::Scene::Ptr scene = create<edtr::EditorScene>(window, simulation);
    // scene->resize();
    // // Main loop
	// while (window.isOpen()) {
    //     // Update
    //     scene->update();
    //     // Render
    //     window.clear(sf::Color(100, 100, 100));
    //     scene->render();
    //     window.display();
	// }

	return 0;
}
