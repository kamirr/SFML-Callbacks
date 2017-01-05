// main.cpp

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

#include "SFCB/ResourceLoader.hpp"

int main()
{
	sf::Clock c;
	sfcb::ResourceLoader& loader = sfcb::ResourceLoader::getInstance();

	for(auto i = 0u; i < 100; ++i) {
		loader.requestStream("sierra.jpg", [i](sf::InputStream& stream) {
			sf::Texture tex;
			tex.loadFromStream(stream);
			std::cout << "Ready (" + std::to_string(i) << ")!\n";
		});
	}
	std::cout << "Started all threads!" << std::endl;

	loader.completeRemainingTasks();

	std::cout << c.getElapsedTime().asSeconds() << std::endl;

	return 0;
}
