#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"
#include "editor/center-text-control.hpp"

#define MAP_NAME "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\map.png"

using namespace edtr;
// using namespace std;

int main()
{
    sf::Clock clock;
	sf::ContextSettings settings;
    sf::Font font;
    font.loadFromFile("res/font.ttf");
    TextControl tc;
	settings.antialiasingLevel = 4;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);

    
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
                window.draw(tc.getStartText(font));
            }

            window.display();
        }
        
        clock.restart();
        while (clock.getElapsedTime().asMilliseconds() < 5000)
        {
            // Update
            scene->update();
            // Render
            window.clear(sf::Color(100, 100, 100));
            scene->render();
            window.draw(tc.getResetText(simulation.vstat.winner, font));
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
