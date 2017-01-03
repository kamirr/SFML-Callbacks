// main.cpp

#include <SFML/System/MemoryInputStream.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <SFML/Audio.hpp>
#include <future>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <thread>

#include "SFCB/NetworkBase.hpp"
#include "SFCB/Callback.hpp"

namespace sfcb {
	class Resources
		: public sf::NonCopyable {
	private:
		std::vector<std::thread> threads;

		static void worker(std::future<void> future) {
			future.wait();
		}

		static void requestBuffer_impl(const std::string path, Callback<const buffer_t&> callback) {
			std::ifstream is(path);
			is >> std::noskipws;

			std::istream_iterator<char> start(is), end;
			buffer_t buffer(start, end);

			callback(buffer);
		}

		Resources()
		{ }

	public:
		static Resources& getInstance() {
			static Resources res;
			return  res;
		}

		void completeRemainingTasks() {
			for(std::thread& t: this->threads)
				t.join();

			this->threads.clear();
		}

		void requestBuffer(const std::string& path, Callback<const buffer_t&> callback) {
			threads.emplace_back(worker, std::async(std::launch::deferred, requestBuffer_impl, path, callback));
		}

		void requestStream(const std::string& path, Callback<sf::MemoryInputStream&> callback) {
			this->requestBuffer(path, [callback](const buffer_t& buffer) {
				sf::MemoryInputStream stream;
				stream.open(buffer.data(), buffer.size());

				callback(stream);
			});
		}
	};
}

int main()
{
	sf::Clock c;

	auto& resources = sfcb::Resources::getInstance();

	for(auto i = 0u; i < 100; ++i)
	resources.requestStream("engine.png", [i](sf::MemoryInputStream& stream) {
		sf::Texture tex;
		tex.loadFromStream(stream);
		std::cout << "Ready " + std::to_string(i) << "!" << std::endl;
	});

	resources.completeRemainingTasks();
	std::cout << c.getElapsedTime().asSeconds() << std::endl;

	return 0;
}
