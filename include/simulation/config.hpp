#pragma once

#include <memory>
#include <filesystem>
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
namespace fs = std::filesystem;

// absolute directory ; directories
#define FONT_DIR "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\font.ttf"
#define RES_DIR "C:\\Users\\Marco\\Desktop\\cpp_ant\\Twitch-Ant-Simulator\\res\\"

template<typename T>
struct DefaultConf
{
	const static sf::Color ANT_COLOR;
	const static sf::Color FOOD_COLOR;
	const static sf::Color TO_FOOD_COLOR;
	const static sf::Color TO_HOME_COLOR;
	const static sf::Color COLONY_COLOR;
	const static sf::Color WALL_COLOR;
	static float COLONY_SIZE;
	static float MARKER_INTENSITY;
	static sf::Vector2f COLONY_POSITION;
	static uint32_t WIN_WIDTH;
	static uint32_t WIN_HEIGHT;
	static uint32_t WORLD_WIDTH;
	static uint32_t WORLD_HEIGHT;
	static uint32_t ANTS_COUNT;
	static std::shared_ptr<sf::Texture> ANT_TEXTURE;
	static std::shared_ptr<sf::Texture> MARKER_TEXTURE;
    static std::shared_ptr<sf::Font> DefaultConf<T>::GLOBAL_FONT;
    static constexpr uint32_t MAX_COLONIES_COUNT = 4;
	static sf::Color COLONY_COLORS[MAX_COLONIES_COUNT];
    static uint32_t USE_FULLSCREEN;
    static float GUI_SCALE;


	static void loadTextures()
	{
        string res(RES_DIR);
        string ant_dir = res + "ant.png";
        string marker_dir = res + "marker.png";

		DefaultConf::ANT_TEXTURE = std::make_shared<sf::Texture>();
		DefaultConf::ANT_TEXTURE->loadFromFile(ant_dir);
		DefaultConf::ANT_TEXTURE->setSmooth(true);
		DefaultConf::MARKER_TEXTURE = std::make_shared<sf::Texture>();
		DefaultConf::MARKER_TEXTURE->loadFromFile(marker_dir);
		DefaultConf::MARKER_TEXTURE->setSmooth(true);
        DefaultConf::GLOBAL_FONT = std::make_shared<sf::Font>();
        DefaultConf::GLOBAL_FONT->loadFromFile(FONT_DIR);
	}

	static void freeTextures()
	{
		DefaultConf::ANT_TEXTURE = nullptr;
		DefaultConf::MARKER_TEXTURE = nullptr;
        DefaultConf::GLOBAL_FONT = nullptr;
	}

    static const std::string chooseMap()
    {
        vector<std::string> maps;
        std::string path = RES_DIR;
        for (const auto & entry : fs::directory_iterator(path))
        {
            if (entry.path().u8string().find("map") != string::npos)
            {
                maps.push_back(entry.path().u8string());
            }
        }

        const std::string test = maps.at(rand() % maps.size());
        return test;
    }
    
    static bool loadUserConf()
    {
        std::ifstream conf_file("conf.txt");
        if (conf_file) {
            cout << "Found configuration file." << endl;
            std::string line;
            uint32_t line_count = 0;
            while (std::getline(conf_file, line)) {
                if (line[0] == '#') {
                    continue;
                }
                const char* line_c = line.c_str();
                switch (line_count) {
                    case 0:
                        DefaultConf<T>::WIN_WIDTH = std::atoi(line_c);
                        break;
                    case 1:
                        DefaultConf<T>::WIN_HEIGHT = std::atoi(line_c);
                        break;
                    case 2:
                        DefaultConf<T>::USE_FULLSCREEN = std::atoi(line_c);
                        break;
                    case 3:
                        DefaultConf<T>::GUI_SCALE = std::atof(line_c);
                        break;
                    case 4:
                        DefaultConf<T>::ANTS_COUNT = std::atoi(line_c);
                        break;
                    default:
                        break;
                }
                ++line_count;
            }
        } else {
            return false;
        }
        return true;
    }
};

template<typename T>
const sf::Color DefaultConf<T>::ANT_COLOR = sf::Color(255, 73, 68);
template<typename T>
const sf::Color DefaultConf<T>::FOOD_COLOR = sf::Color(66, 153, 66);
template<typename T>
const sf::Color DefaultConf<T>::TO_FOOD_COLOR = sf::Color(0, 255, 0);
template<typename T>
const sf::Color DefaultConf<T>::TO_HOME_COLOR = sf::Color(255, 0, 0);
template<typename T>
const sf::Color DefaultConf<T>::COLONY_COLOR = DefaultConf<T>::ANT_COLOR;
template<typename T>
const sf::Color DefaultConf<T>::WALL_COLOR = sf::Color(105, 47, 14);
template<typename T>
uint32_t DefaultConf<T>::WIN_WIDTH = 1920;
template<typename T>
uint32_t DefaultConf<T>::WIN_HEIGHT = 1080;
template<typename T>
uint32_t DefaultConf<T>::WORLD_WIDTH = 1920;
template<typename T>
uint32_t DefaultConf<T>::WORLD_HEIGHT = 1080;
template<typename T>
uint32_t DefaultConf<T>::ANTS_COUNT = 4000; // Maximum per colony.
template<typename T>
float DefaultConf<T>::COLONY_SIZE = 20.0f;
template<typename T>
float DefaultConf<T>::MARKER_INTENSITY = 8000.0f;
template<typename T>
sf::Vector2f DefaultConf<T>::COLONY_POSITION = sf::Vector2f(500.0f, DefaultConf<T>::WIN_HEIGHT * 0.5f);

template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::ANT_TEXTURE;
template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::MARKER_TEXTURE;
template<typename T>
std::shared_ptr<sf::Font> DefaultConf<T>::GLOBAL_FONT;
template<typename T>
sf::Color DefaultConf<T>::COLONY_COLORS[MAX_COLONIES_COUNT] = {sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Cyan};

template<typename T>
uint32_t DefaultConf<T>::USE_FULLSCREEN = 1;

template<typename T>
float DefaultConf<T>::GUI_SCALE = 1.0f;

using Conf = DefaultConf<int>;