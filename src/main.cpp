#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"

#define MAP_NAME "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\map.png"

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

    while (window.isOpen())
    {
        // Initialize simulation
        Simulation simulation(window);
        simulation.loadMap(MAP_NAME);
        // Create editor scene around it
        GUI::Scene::Ptr scene = create<edtr::EditorScene>(window, simulation);
        scene->resize();

        while (simulation.isRunning) // isRunning should be set to false when the win condition is implemented.
        {
            // Update
            scene->update();
            // Render
            window.clear(sf::Color(100, 100, 100));
            scene->render();
            window.display();
        }

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
