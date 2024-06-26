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
#include "editor/center-text-control.hpp"
#include "world/async_distance_field_builder.hpp"
#include "IPC/client-controller.h"
#include "IPC/client-controller.cpp"
#include "victorystatus.hpp"
#include <vector>
#include <tuple>
#include <iostream>
#include <cmath>

#define MAP_NAME "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\map.png"

struct Simulation
{
	civ::Vector<Colony> colonies;
    vector<uint8_t> extinct_colonies;
	World world;
	Renderer renderer;
	EventSate ev_state;
	FightSystem fight_system;
	sf::Clock clock;
    AsyncDistanceFieldBuilder distance_field_builder;
    client_controller cl_cont;
    vector<tuple<float, float>> spawnPoints;
    VictoryStatus vstat;
    unordered_map<string, uint64_t> tracked_populations;
    TextControl tc;

    sf::Text pollstr = tc.getDefaultText();
    bool isRunning = true;
    int num_cols = Conf::MAX_COLONIES_COUNT;
    bool earlyStoppingPoll = false;
    bool finalTwo = false;

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
               int quantity = stoi(cmd.substr(2));
               bool found = true;
               Colony& c = findColonyByColor(color, &found);

               if (!found)
                continue;

               if (quantity == 1)
               {
                    c.createWorker();
               }
               else
               {
                   c.createNWorker(quantity);
                   pollstr = tc.getPollText('S', c.getColorString());
               }

               cout << "Spawned ant" << endl;
           }
           else if (cmd.at(0) == 'F') // spawn food
           {
               char color = cmd.at(1);
               int quantity = stoi(cmd.substr(2));
               bool found = true;
               Colony& c = findColonyByColor(color, &found);

               if (!found)
                continue;

               if (quantity == 1)
               {
                    sf::Vector2f coords = c.base.position;
                    sf::Vector2f new_coords = c.radialNoise(coords, 75); // 125px radius?
                    world.addFoodAt(new_coords.x, new_coords.y, 10); // spawn food at coords
               }
               else
               {
                   vector<sf::Vector2f> pos = c.set_radialNoise(c.base.position, 75, quantity);
                   world.addNFoodAt(pos, 10, quantity);
                   pollstr = tc.getPollText('F', c.getColorString());
               }
           }
           else if (cmd.at(0) == 'K') // No quantity check necessary as these will only be sent by server.
           {
               char color = cmd.at(1);
               int quantity = stoi(cmd.substr(2));
               bool found = true;
               Colony& c = findColonyByColor(color, &found);

               if (!found)
                continue;

               c.killNAnts(world, quantity);
               pollstr = tc.getPollText('K', c.getColorString());
           }
           else if (cmd.at(0) == 'Q')
           {
               char color = cmd.at(1);
               int quantity = stoi(cmd.substr(2));
               bool found = true;
               Colony& c = findColonyByColor(color, &found);

               if (!found)
                continue;

               c.increaseAntSpeed(quantity / 1.0); 
               pollstr = tc.getPollText('Q', c.getColorString());
           }
           else if (cmd.at(0) == 'M')
           {
               char color = cmd.at(1);
               int quantity = stoi(cmd.substr(2));
               bool found = true;
               Colony& c = findColonyByColor(color, &found);

               if (!found)
                continue;

               c.increaseSpawnRate(quantity / 10.0);
               pollstr = tc.getPollText('M', c.getColorString());
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
           else if (cmd.at(0) == 'E')
           {
               earlyStoppingPoll = false;
               string stop = cmd.substr(1);
               if (stop == "Yes") // Restart if poll is YES.
               {
                   vstat.interrupted = true;
                   isRunning = false;
                   break;
               }
           }
           else
           {
               return; // Maybe remove this?
           }

        }
        
        if (commands.size() > 0)
        {
            cout << "Commands processed: " << commands.size() << endl;
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
            if (cl_cont.isReady())
            {
                vector<string> temp = cl_cont.fetch();
                processCommands(temp, dt);
                temp.clear();
            }
            else
            {
                vector<string> temp = {};
                processCommands(temp, dt);
            }

            // Implement early stopping. If there are fewer than n deaths in 30 seconds, create poll.
            if (finalTwo == true && static_cast<int>((clock.getElapsedTime().asSeconds())) > 90 && earlyStoppingPoll == false)
            {
                cout << "Early stopping reached." << endl;
                const float DELTA = 50.0;
                float total = 0;
                
                // Look through all colonies and check population change.
                for (Colony& colony: colonies)
                {
                    if (colony.ants.size() > 0 && !isExtinct(colony.id))
                    {
                        if (tracked_populations[colony.getColorString()] != -1)
                        {
                            int temp = colony.ants.size() - tracked_populations[colony.getColorString()];
                            total += abs(to<float>(temp));
                        }
                    }
                    
                    tracked_populations[colony.getColorString()] = colony.ants.size();
                }

                // If both colonies saw fewer than NET_LOSS, start poll.
                if (total < DELTA)
                {
                    cout << "early stopping: " << total << endl;
                    earlyStoppingPoll = true;
                    cl_cont.server_Create_Poll("!" + getCurrentColoniesStr());
                }
                else
                {
                    clock.restart();
                }
            }

            // Check if any colonies are all out of ants. Mark them as dead.
            for (Colony& colony : colonies)
            {
                if (colony.ants.size() == 0 && !isExtinct(colony.id))
                {
                    extinct_colonies.push_back(colony.id);
                    colony.setColor(sf::Color(0,0,0,0));
                    world.addFoodAt(colony.base.position.x, colony.base.position.y, colony.base.food);
                    colony.base.food = 0;
                    num_cols--;
                }

                // Only two colonies remain. Start clock and initialize population tracking.
                if (num_cols == 2 && finalTwo == false)
                {
                    for (Colony& colony: colonies)
                    {
                        if (colony.ants.size() != 0 && !isExtinct(colony.id))
                        {
                            tracked_populations[colony.getColorString()] = colony.ants.size();
                        }
                    }

                    clock.restart();
                    finalTwo = true;
                }
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
				colony.genericAntsUpdate(dt, world);
			}
			// Then update objectives and world sampling
			for (Colony& colony : colonies) {
				colony.update(dt, world);
			}

            if (num_cols == 1) // End simulation if only one colony remains.
            {
                for (Colony& colony : colonies)
                {
                    if (!isExtinct(colony.id))
                    {
                        cout << colony.getColorString() << endl;
                        vstat.winner = colony.getColorString();
                        break;
                    }
                }

                isRunning = false;
            }

			// Search for fights
			fight_system.checkForFights(colonies, world);
			// Update stats
			renderer.updateColoniesStats(dt);
		}
	}

    string getCurrentColoniesStr()
    {
        string ret = "";

        for (Colony& c : colonies)
        {
            if (c.getColor() == sf::Color::Red)
            {
                ret += "r";
            }
            else if (c.getColor() == sf::Color::Blue)
            {
                ret += "b";
            }
            else if (c.getColor() == sf::Color::Green)
            {
                ret += "g";
            }
            else if (c.getColor() == sf::Color::Cyan)
            {
                ret += "c";
            }
        }

        return ret;
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

    void removeColony_death(uint8_t colony_id)
    {
        colonies.erase(colony_id);
        renderer.colonies.erase(colony_id);
        world.renderer.colonies_color.erase(colony_id);
        world.clearMarkers(colony_id);
    }

    void removeColony(uint8_t colony_id)
    {
        for (Colony& c : colonies) {
            if (c.id == colony_id)
                continue;

            cout << c.getColorString() << " stopping fights with " << unsigned(colony_id) << endl;
            c.stopFightsWith(colony_id);
        }

        num_cols--;
        extinct_colonies.push_back(colony_id);
        colonies.erase(colony_id);
        //rescaleIDs(colony_id);
        renderer.colonies.erase(colony_id);
        world.renderer.colonies_color.erase(colony_id);
        world.clearMarkers(colony_id);
    }

    bool isExtinct(uint8_t colony_id)
    {
        if (std::find(extinct_colonies.begin(), extinct_colonies.end(), colony_id) != extinct_colonies.end()) {
            return true;
        }
        else {
            return false;
        }
    }
};