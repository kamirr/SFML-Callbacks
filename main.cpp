// main.cpp

#include "SFCB/TcpListener.hpp"
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

	void onDataReceived(sfcb::TcpEvent ev, std::string& log) {
		for(const sf::Int8 c: *ev.buffer)
			log += c;

		std::cout << "Log updated:\n" << log << std::endl;
	}

	void onAccepted(sfcb::sharedTcpSocket socket, std::vector<sfcb::sharedTcpSocket>& clients, std::string& log) {
		std::cout << "connected (" << (*socket).getRemoteAddress() << ")" << std::endl;

		static char arr[] = "Hi!\n";
		sfcb::buffer_t buffer(std::begin(arr), std::end(arr));
		(*socket).send(buffer);

		(*socket).setCallback(sfcb::TcpEvent::DataReceived, onDataReceived, std::ref(log));
		clients.push_back(socket);
	}
}

int main()
{
	/* Create window, acts just like window given in parameter */
	sfcb::Window<sf::RenderWindow> app({300, 300}, "app");
	app.setFramerateLimit(60);

	app.setCallback(sf::Event::Closed, app.getUniversalCallbackContext(), callbacks::onClose);

	std::vector<sfcb::sharedTcpSocket> clients;
	std::string log;

	sfcb::TcpListener listener;
	listener.setCallback(callbacks::onAccepted, std::ref(clients), std::ref(log));
	listener.listen(3254);

	/* Minimal main loop */
	app.clear({20, 20, 20});
	while(app.isOpen()) {
		/* Handle window callbacks */
		app.handleCallbacks();

		/* Handle listener callbacks */
		listener.handleCallbacks();

		/* Handle TCP callbacks */
		sfcb::TcpSocket::handleCallbacks();

		/* All callbacks are called right before displaying window */
		app.display();
	}

	return 0;
}
