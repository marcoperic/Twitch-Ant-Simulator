#pragma once
#include "world/world.hpp"
#include "colony/colony.hpp"
#include "config.hpp"
#include "common/viewport_handler.hpp"
#include "common/event_manager.hpp"
#include "event_state.hpp"
#include "render/renderer.hpp"
#include "simulation/world/map_loader.hpp"
#include "simulation/ant/fight_system.hpp"
#include "world/async_distance_field_builder.hpp"
#include "IPC/client-controller.h"
#include "IPC/client-controller.cpp"
#include "victorystatus.hpp"
#include <vector>
#include <tuple>
#include <iostream>

#define MAP_NAME "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\map.png"

struct Simulation
{
	civ::Vector<Colony> colonies;
	World world;
	Renderer renderer;
	EventSate ev_state;
	FightSystem fight_system;
	sf::Clock clock;
    AsyncDistanceFieldBuilder distance_field_builder;
    client_controller c;
    vector<tuple<float, float>> spawnPoints;
    VictoryStatus vstat;
    bool isRunning = true;

    explicit
	Simulation(sf::Window& window)
		: world(Conf::WORLD_WIDTH, Conf::WORLD_HEIGHT)
		, renderer()
        , distance_field_builder(world.map)
	{
	}

	void loadMap(const std::string& map_filename)
	{
		MapLoader::loadMap(world, map_filename, spawnPoints);
        distance_field_builder.requestUpdate();
	}

	civ::Ref<Colony> createColony(float colony_x, float colony_y)
	{
		// Create the colony object
		const civ::ID colony_id = colonies.emplace_back(colony_x, colony_y, Conf::ANTS_COUNT);
		auto colony_ref = colonies.getRef(colony_id);
        Colony& colony = *colony_ref;
        colony.initialize(to<uint8_t>(colony_id));
		// Create colony markers
        createColonyMarkers(colony);
		// Register it for the renderer
		renderer.addColony(colony_ref);
        world.renderer.colonies_color.emplace_back();

        return colony_ref;
	}

    void processCommands(vector<string> commands, float dt)
    {
        for (string cmd: commands)
        {
           cout << cmd << endl;
           if (cmd.at(0) == 'S') // spawn ant
           {
               char color = cmd.at(1);
               bool found = true;
               Colony& c = findColonyByColor(color, &found);

               if (!found)
                continue;

               c.createWorker();
               cout << "Spawned ant" << endl;
           }
           else if (cmd.at(0) == 'F') // spawn food
           {
               char color = cmd.at(1);
               bool found = true;
               Colony& c = findColonyByColor(color, &found);

               if (!found)
                continue;

               sf::Vector2f coords = c.base.position;
               sf::Vector2f new_coords = c.radialNoise(coords, 125); // 125px radius?
               world.addFoodAt(new_coords.x, new_coords.y, 10); // spawn food at coords
           }
           else if (cmd.at(0) == '@')
           {
               string command = cmd.substr(1);
               if (command == "restart")
               {
                   vstat.interrupted = true;
                   isRunning = false;
                   break;
               }
           }
           else
           {
               return;
           }
        }
    }

    Colony& findColonyByColor(char s, bool *found)
    {
        sf::Color c;
        if (s == 'r')
            c = sf::Color::Red;
        else if (s == 'b')
            c = sf::Color::Blue;
        else if (s == 'g')
            c = sf::Color::Green;
        else if (s == 'c')
            c = sf::Color::Cyan;

        for (Colony& col : colonies)
        {
            if (col.ants_color == c)
            {
                return col;
            }
        }

		cout << "Colony is not active." << endl;
        *found = false;
    }

	void update(float dt)
	{
        // std::cout << "Update";
		if (!ev_state.pause) {
            // Mark ants with no more time left as dead
            removeDeadAnts();

            // Get incoming data from queue structure to perform operations based on chat interaction
            if (c.isReady())
            {
                vector<string> temp = c.fetch();
                processCommands(temp, dt);
                temp.clear();
            }
            else
            {
                vector<string> temp = {};
                processCommands(temp, dt);
            }

			// Update world cells (markers, density, walls)
			world.update(dt);
			// First perform position update and grid registration
            for (Colony& colony : colonies) {
                if (colony.color_changed) {
                    world.renderer.colonies_color[colony.id] = colony.ants_color;
                }
                if (colony.position_changed) {
                    updateColonyPosition(colony);
                }
            }

			for (Colony& colony : colonies) {
                // cout << unsigned(colony.id) << endl; // print the numerical code.
				colony.genericAntsUpdate(dt, world);
			}
			// Then update objectives and world sampling
			for (Colony& colony : colonies) {
				colony.update(dt, world);
			}

            for (Colony& colony: colonies)
            {
                if (colony.ants.size() == 0)
                {
                    removeColony(colony.id);
                }
            }

            if (colonies.size() == 1)
            {
                // Only one colony left? End simulation.
                //vstat.winner = colonies.get(0).getColorString();
                isRunning = false;
            }

            // Verify that commit #af0375701873821ded1be7f431ba4384d99f640e works
            // for (Colony& colony : colonies)
            // {
            //     if (colony.ants.size() > 135)
            //     {
            //         vstat.winner = colony.getColorString();
            //         isRunning = false;
            //         break;
            //     }
            // }

			// Search for fights
			fight_system.checkForFights(colonies, world);
			// Update stats
			renderer.updateColoniesStats(dt);
		}
	}
    
    void removeDeadAnts()
    {
        // Mark old ants as dead
        for (Colony& colony : colonies) {
            const uint32_t killed = colony.killWeakAnts(world);
            if (killed) {
                const uint32_t initial_size = to<int32_t>(colony.ants.size());
                renderer.colonies[colony.id].cleanVAs(initial_size - killed, initial_size);
            }
        }
        // Remove them
        for (Colony& colony : colonies) {
            colony.removeDeadAnts();
        }
    }

    void updateColonyPosition(Colony& colony)
    {
        colony.position_changed = false;
        world.clearMarkers(colony.id);
        // Create colony markers
        createColonyMarkers(colony);
    }

    void createColonyMarkers(Colony& colony)
    {
        for (uint32_t i(0); i < 64; ++i) {
            float angle = float(i) / 64.0f * (2.0f * PI);
            world.addMarker(colony.base.position + 0.9f * colony.base.radius * sf::Vector2f(cos(angle), sin(angle)), Mode::ToHome, 10.0f, true);
        }
    }

	void render(sf::RenderTarget& target)
	{
		renderer.render(world, target);
	}

    void removeColony(uint8_t colony_id)
    {

        for (Colony& c : colonies) {
            c.stopFightsWith(colony_id);
        }
        colonies.erase(colony_id);
        renderer.colonies.erase(colony_id);
        world.renderer.colonies_color.erase(colony_id);
        world.clearMarkers(colony_id);
    }
};
