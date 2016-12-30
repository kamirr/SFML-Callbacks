#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <SFML/Network/UdpSocket.hpp>
#include <functional>
#include <algorithm>
#include <vector>

#include "NetworkBase.hpp"
#include "Callback.hpp"

namespace sfcb {
	class UdpSocket
	: public sf::NonCopyable {
	private:
		Callback<const buffer_t&> m_onDataReceived;
		Callback<SocketStatus> m_onError;

		static std::vector<UdpSocket*> sockets;
		sf::UdpSocket m_socket;

	public:
		UdpSocket() {
			m_socket.setBlocking(false);
			sockets.push_back(this);
		}
		~UdpSocket() {
			auto it = std::find(sockets.begin(), sockets.end(), this);
			sockets.erase(it);
		}

		unsigned short getLocalPort() const {
			return this->m_socket.getLocalPort();
		}

		SocketStatus bind(unsigned short port, const sf::IpAddress &address = sf::IpAddress::Any) {
			return this->m_socket.bind(port, address);
		}

		void unbind() {
			this->m_socket.unbind();
		}

		void send(const buffer_t& buffer, const sf::IpAddress &remoteAddress, unsigned short remotePort) {
			auto status = this->m_socket.send(buffer.data(), buffer.size(), remoteAddress, remotePort);

			if(status != SocketStatus::Done) {
				this->m_onError(status);
			}
		}

		template<typename callback_t, typename ... args_t>
		void onDataReceived(callback_t func, const args_t& ... args) {
			this->m_onDataReceived.set(func, args ...);
		}

		template<typename callback_t, typename ... args_t>
		void onError(callback_t func, const args_t& ... args) {
			this->m_onError.set(func, args ...);
		}

		static void handleCallbacks() {
			for(auto* ptr: sockets) {
				auto& socket = ptr->m_socket;

				buffer_t data;
				SocketStatus status;

				do {
					char buffer[1024];
					unsigned short port;
					sf::IpAddress sender;
					std::size_t received = 0;

					status = socket.receive(buffer, sizeof(buffer), received, sender, port);

					for(auto i = 0u; i < received; ++i) {
						data.push_back(buffer[i]);
					}
				} while(status == SocketStatus::Partial);

				if(status == SocketStatus::Done)
					ptr->m_onDataReceived(data);
				else if(status != SocketStatus::NotReady)
					ptr->m_onError(status);
			}
		}
	};

	std::vector<UdpSocket*> UdpSocket::sockets;
}

#endif // UDPSOCKET_H
