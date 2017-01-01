// main.cpp

#include "SFCB/TcpListener.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

/* Namespace with callbacks */
namespace callbacks {
	void onAccepted(sfcb::sharedTcpSocket socket, std::vector<sfcb::sharedTcpSocket>& clients, std::string& log) {
		std::cout << "connected (" << (*socket).getRemoteAddress() << ")" << std::endl;

		static const std::string charset("abcdefghiklmonpqrstuwxyzABCDEFGHIKLMNOPQRSTUWXYZ0123456789");
		std::string login;

		int r = rand() % 255, g = rand() % 255;
		auto background = "\033[48;2;" + std::to_string(r)     + ";" + std::to_string(g)     + ";" + std::to_string(255) + "m";
		auto foreground = "\033[38;2;" + std::to_string(255-r) + ";" + std::to_string(255-g) + ";" + std::to_string(0)   + "m";
		login = background + foreground;

		for(int i = 0; i < 4; ++i) {
			login += charset[size_t(rand()) % charset.size()];
		}

		login += ":\033[0m ";

		static char arr[] = "Hi!\n";
		sfcb::buffer_t buffer(std::begin(arr), std::end(arr));

		(*socket).send(buffer);
		(*socket).setCallback(sfcb::SocketEvent::DataReceived, [&log, login](sfcb::SocketEvent ev) {
			log += login;
			for(const sf::Int8 c: *ev.buffer) {
				log += char(c);
			}
		});

		clients.push_back(socket);
	}
}

int main()
{
	std::vector<sfcb::sharedTcpSocket> clients;
	std::srand(std::time(nullptr));

	std::string log;
	size_t oldSize = log.size();

	sfcb::TcpListener listener;
	listener.setCallback(callbacks::onAccepted, std::ref(clients), std::ref(log));
	listener.listen(3254);

	while(true) {
		/* Handle listener callbacks */
		listener.handleCallbacks();

		/* Handle TCP callbacks */
		sfcb::TcpSocket::handleCallbacks();

		if(oldSize != log.size()) {
			system("clear");
			std::cout << log << std::flush;

			oldSize = log.size();
		}
	}

	return 0;
}
