// main.cpp

#include "SFCB/TcpSocket.hpp"
#include "SFCB/Window.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <sstream>

#include <SFML/Network/TcpListener.hpp>
#include <memory>
namespace sfcb {
	typedef std::shared_ptr<TcpSocket> sharedTcpSocket;

	class TcpListener {
	private:
		Callback<sharedTcpSocket> m_onAccepted;
		sf::TcpListener m_listener;

	public:
		TcpListener() {
			this->m_listener.setBlocking(false);
		}

		unsigned short getLocalPort() {
			return this->m_listener.getLocalPort();
		}

		SocketStatus listen(unsigned short port) {
			return this->m_listener.listen(port);
		}

		void close() {
			this->m_listener.close();
		}

		template<typename func_t, typename ... args_t>
		void onAccepted(func_t func, const args_t& ... args) {
			this->m_onAccepted.set(func, args ...);
		}

		void handleCallbacks() {
			auto client = std::make_shared<TcpSocket>();
			auto status = this->m_listener.accept((*client).m_socket);

			if(status == SocketStatus::Done)
				this->m_onAccepted(client);
		}
	};
}

/* Namespace with callbacks */
namespace callbacks {
	void onClose(sf::RenderWindow& window, sf::Event) {
		std::cout << "Y u do dis? ;-;" << std::endl;
		window.close();
	}

	void onDataReceived(const sfcb::buffer_t& data, std::string& log) {
		for(const sf::Int8 c: data)
			log += c;

		std::cout << "Log updated:\n" << log << std::endl;
	}

	void onAccepted(sfcb::sharedTcpSocket socket, std::vector<sfcb::sharedTcpSocket>& clients, std::string& log) {
		std::cout << "connected (" << (*socket).getRemoteAddress() << ")" << std::endl;

		static char arr[] = "Hi!\n";
		sfcb::buffer_t buffer(std::begin(arr), std::end(arr));
		(*socket).send(buffer);

		(*socket).onDataReceived(onDataReceived, std::ref(log));
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
	listener.onAccepted(callbacks::onAccepted, std::ref(clients), std::ref(log));
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
