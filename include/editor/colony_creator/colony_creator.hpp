#pragma once
#include "editor/GUI/container.hpp"
#include "editor/GUI/button.hpp"
#include "colony_tool.hpp"
#include "simulation/simulation.hpp"
#include <vector>
#include <iostream>


namespace edtr
{

struct ColonyCreator : public GUI::NamedContainer
{
    Simulation&   simulation;
    ControlState& control_state;
    uint32_t      colonies_count = 0;
    int32_t       last_selected = -1;

    explicit
    ColonyCreator(Simulation& sim, ControlState& control_state_)
        : GUI::NamedContainer("Colonies", Container::Orientation::Vertical)
        , simulation(sim)
        , control_state(control_state_)
    {
        root->size_type.y = GUI::Size::FitContent;
        // auto add_button = create<GUI::Button>("Add", [this](){
        //     // this->createColony();
        //     this->spawnColonies();
        // });

        this->spawnColonies();
        // add_button->setWidth(32.0f, GUI::Size::Fixed);
        // add_button->setHeight(20.0f, GUI::Size::Fixed);
        header->addItem(create<GUI::EmptyItem>());
        // header->addItem(add_button);
        
    }

    void spawnColonies()
    {
        for (tuple<float, float> col: simulation.spawnPoints)
        {
            do
            {
                tuple<float, float> temp = radialNoise(col, 300); // 300px radius of spawning possibility.

                if (!(get<0>(temp) < Conf::WIN_WIDTH && get<1>(temp) < Conf::WIN_HEIGHT) || !(get<0>(temp) > 0 && get<1>(temp) > 0))
                    continue;

                const auto& cell = simulation.world.map.getCst(sf::Vector2f(get<0>(temp), get<1>(temp)));
                if (cell.wall || cell.food)
                    continue;

                if (radiusIntersectsWall(temp, 12)) // 12px clearance from food and wall.
                    continue;

                col = temp;
                break;

            } while (true);


            this->createColony(get<0>(col), get<1>(col));
        }
    }

    bool radiusIntersectsWall(tuple<float, float> c, int radius)
    {
        for (int i = 1; i <= 360; i++)
        {
            float radians = i * PI / 180;
            tuple<float, float> point = { get<0>(c) + radius * cos(radians), get<1>(c) + radius * sin(radians) };
            const auto& cell = simulation.world.map.getCst(sf::Vector2f(get<0>(point), get<1>(point)));
            if (cell.wall || cell.food)
                return true;
        }

        return false;
    }

	tuple<float, float> radialNoise(tuple<float, float> v, float radius)
	{
		float rannum = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float r = radius * sqrt(rannum);
		float theta = rannum * 2 * PI;
		float x = (get<0>(v) + r * cos(theta));
		float y = (get<1>(v) + r * sin(theta));
		return {x, y};
	}

    void createColony(float x, float y)
    {
        if (this->colonies_count < Conf::MAX_COLONIES_COUNT) {
            sf::Color temp;
            auto new_colony = simulation.createColony(x, y);
            auto colony_tool = create<ColonyTool>(new_colony, control_state);
            colony_tool->setColor(temp = assignColor());
            colony_tool->colony->ants_color = temp;
            //colony_tool->stats_toggle->setState(true);
            colony_tool->on_select = [this](int8_t id){
                select(id);
            };
            // Add the new item to this
            this->addItem(colony_tool);
            ++this->colonies_count;

            // Set the correct callback for the remove button
            colony_tool->top_zone->getByName<GUI::Button>("remove")->click_callback = [this, colony_tool](){
                //resetColor(colony_tool->colony->ants_color);
                simulation.removeColony(colony_tool->colony->id);
                this->removeItem(colony_tool);
                --this->colonies_count;
            };
        }
    }

    sf::Color assignColor()
    {
        if (simulation.colonies.size() == 0)
            return sf::Color::Red;

        bool colors[4] = {false, false, false, false};
        for (Colony& c: simulation.colonies)
        {
            if (c.ants_color == sf::Color::Red)
            {
                colors[0] = true;
            }
            else if (c.ants_color == sf::Color::Blue)
            {
                colors[1] = true;
            }
            else if (c.ants_color == sf::Color::Green)
            {
                colors[2] = true;
            }
            else if (c.ants_color == sf::Color::Cyan)
            {
                colors[3] = true;
            }
        }

        int i;
        for (i = 0; i < 4; i++)
        {
            if (colors[i] == false)
                break;
        }

        if (i == 1)
            return sf::Color::Blue;
        else if (i == 2)
            return sf::Color::Green;
        else if (i == 3)
            return sf::Color::Cyan;

        return sf::Color::Red;
    }

    void select(int32_t id)
    {
        int32_t selected = -1;
        if (id != last_selected) {
            selected = id;
        }
        last_selected = selected;
        this->simulation.world.renderer.selected_colony = selected;

        for (const auto& item : root->sub_items) {
            auto tool = std::dynamic_pointer_cast<ColonyTool>(item);
            if (tool) {
                tool->selected = tool->colony->id == selected;
            }
        }
    }
};

}

