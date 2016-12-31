#ifndef TCPLISTENER_HPP
#define TCPLISTENER_HPP

#include <SFML/Network/TcpListener.hpp>
#include <memory>

#include "TcpSocket.hpp"

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
		void setCallback(func_t func, const args_t& ... args) {
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

#endif // TCPLISTENER_HPP
