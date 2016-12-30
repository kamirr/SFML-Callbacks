// main.cpp

#include "SFCB/Window.hpp"
#include "SFCB/UdpSocket.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <sstream>

/* Namespace with callbacks */
namespace callbacks {
	void onClose(sf::RenderWindow& window, sf::Event) {
		std::cout << "Y u do dis? ;-;" << std::endl;
		window.close();
	}

	void onReceive(const std::vector<sf::Int8>& buffer, sf::RenderWindow& window) {
		std::cout << "Received " << buffer.size() << " bytes.\n";
		if(buffer.size() == 11) {
			std::istringstream in(std::string(buffer.begin(), buffer.end()));
			int r, g, b;
			in >> r >> g >> b;

			window.clear({
				sf::Uint8(r),
				sf::Uint8(g),
				sf::Uint8(b)
			});
		}
	}
}

int main()
{
	/* Create window, acts just like window given in parameter */
	sfcb::Window<sf::RenderWindow> app({300, 300}, "app");
	app.setFramerateLimit(60);

	app.setCallback(sf::Event::Closed, app.getUniversalCallbackContext(), callbacks::onClose);

	sfcb::UdpSocket socket;
	socket.bind(3264);
	socket.setCallback(callbacks::onReceive, app);

	/* Minimal main loop */
	app.clear({20, 20, 20});
	while(app.isOpen()) {
		/* Handle window callbacks */
		app.handleCallbacks();

		/* Handle network callbacks */
		sfcb::UdpSocket::handleCallbacks();

		/* All callbacks are called right before displaying window */
		app.display();
	}

	return 0;
}
