// main.cpp

#include "SFCB/TcpSocket.hpp"
#include "SFCB/Window.hpp"

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

	void onError(const sf::Socket::Status& status, std::ostream& out) {
		out << "Socket: ";

		switch (status) {
		case sf::Socket::Disconnected:
			out << "disconnected";
			break;
		case sf::Socket::Error:
			out << "error";
			break;
		case sf::Socket::NotReady:
			out << "not ready";
			break;
		default:
			out << "[unknown " << size_t(status) << "]";
		}

		out << std::endl;
		exit(1);
	}

	void onConnected(sfcb::TcpSocket& socket) {
		static char arr[] = "lel\n";
		std::vector<sf::Int8> buffer(std::begin(arr), std::end(arr));
		socket.send(buffer);
	}
}

int main()
{
	/* Create window, acts just like window given in parameter */
	sfcb::Window<sf::RenderWindow> app({300, 300}, "app");
	app.setFramerateLimit(60);

	app.setCallback(sf::Event::Closed, app.getUniversalCallbackContext(), callbacks::onClose);

	sfcb::TcpSocket socket;

	/* setCallback method takes parameters by value,
	 * so std::ref is required to explictly pass reference */
	socket.onDataReceived(callbacks::onReceive, std::ref(app));
	socket.onError(callbacks::onError, std::ref(std::cout));
	socket.onConnected(callbacks::onConnected);

	socket.connect("localhost", 3264);

	/* Minimal main loop */
	app.clear({20, 20, 20});
	while(app.isOpen()) {
		/* Handle window callbacks */
		app.handleCallbacks();

		/* Handle network callbacks */
		sfcb::TcpSocket::handleCallbacks();

		/* All callbacks are called right before displaying window */
		app.display();
	}

	return 0;
}
