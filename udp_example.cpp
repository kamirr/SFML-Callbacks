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

	void onReceive(sfcb::SocketEvent ev, sf::RenderWindow& window) {
		std::cout << "Received " << ev.buffer->size() << " bytes.\n";
		if(ev.buffer->size() == 11) {
			std::istringstream in(std::string(ev.buffer->begin(), ev.buffer->end()));
			int r, g, b;
			in >> r >> g >> b;

			window.clear({
				sf::Uint8(r),
				sf::Uint8(g),
				sf::Uint8(b)
			});
		}
	}

	void onError(sfcb::SocketEvent ev, std::ostream& out) {
		out << "Socket: ";

		switch (ev.status) {
		case sfcb::SocketStatus::Disconnected:
			out << "disconnected";
			break;
		case sfcb::SocketStatus::Error:
			out << "error";
			break;
		default:
			out << "[unknown]";
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

	char arr[] = "lel\n";
	sfcb::buffer_t buffer(std::begin(arr), std::end(arr));
	socket.send(buffer, "127.0.0.1", 3264);

	/* setCallback method takes parameters by value,
	 * so std::ref is required to explictly pass reference */
	socket.setCallback(sfcb::SocketEvent::DataReceived, callbacks::onReceive, std::ref(app));
	socket.setCallback(sfcb::SocketEvent::Error, callbacks::onError, std::ref(std::cout));

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
