// main.cpp

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

#include "SFCB/ResourceLoader.hpp"

int main()
{
	sf::Clock c;
	sfcb::ResourceLoader& loader = sfcb::ResourceLoader::getInstance();
	loader.enableAsync(true);

	for(auto i = 0u; i < 100; ++i) {
		loader.requestStream("sierra1.png", [i](sf::InputStream& stream) {
			sf::Texture tex;
			tex.loadFromStream(stream);
			std::cout << "Ready (" + std::to_string(i) << ")!\n";
		});
	}
	std::cout << "Started all threads!" << std::endl;

	loader.completeRemainingTasks();

	std::cout << c.getElapsedTime().asSeconds() << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));

	return 0;
}
