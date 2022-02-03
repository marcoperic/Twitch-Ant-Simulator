#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"

#define MAP_NAME "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\map_test.png"

int main()
{
    // Load configuration
    if (Conf::loadUserConf()) {
        std::cout << "Configuration file loaded." << std::endl;
    } else {
        std::cout << "Configuration file couldn't be found." << std::endl;
    }

	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);
    // Initialize simulation
    Simulation simulation(window);
    simulation.loadMap(MAP_NAME);
    // simulation.createColony(Conf::WIN_WIDTH / 2 , Conf::WIN_HEIGHT / 2); // Creates a single colony off the rip
    // Create editor scene around it
    GUI::Scene::Ptr scene = create<edtr::EditorScene>(window, simulation);
    scene->resize();
    // Main loop
	while (window.isOpen()) {
        // Update
        scene->update();
        // Render
        window.clear(sf::Color(100, 100, 100));
        scene->render();
        window.display();
	}

	return 0;
}
