#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <SFML/Network/UdpSocket.hpp>
#include <functional>
#include <algorithm>
#include <vector>

namespace sfcb {
	class UdpSocket
	: public sf::NonCopyable {
	private:
		std::function<void(const std::vector<sf::Int8>&)> m_onDataReceived;
		std::function<void(const sf::UdpSocket::Status)> m_onError;

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

		sf::UdpSocket::Status bind(unsigned short port, const sf::IpAddress &address = sf::IpAddress::Any) {
			return this->m_socket.bind(port, address);
		}

		void unbind() {
			this->m_socket.unbind();
		}

		void send(const std::vector<sf::Int8>& buffer, const sf::IpAddress &remoteAddress, unsigned short remotePort) {
			auto status = this->m_socket.send(buffer.data(), buffer.size(), remoteAddress, remotePort);

			if(status != sf::UdpSocket::Done) {
				this->m_onError(status);
			}
		}

		template<typename callback_t, typename ... args_t>
		void onDataReceived(callback_t callback, const args_t& ... args) {
			this->m_onDataReceived = [callback, args ...](const std::vector<sf::Int8>& vec) {
				callback(vec, args ...);
			};
		}

		template<typename callback_t, typename ... args_t>
		void onError(callback_t callback, const args_t& ... args) {
			this->m_onError = [callback, args ...](const sf::UdpSocket::Status& status) {
				callback(status, args ...);
			};
		}

		static void handleCallbacks() {
			for(auto* ptr: sockets) {
				auto& socket = ptr->m_socket;

				std::vector<sf::Int8> data;
				sf::UdpSocket::Status status;

				do {
					char buffer[1024];
					unsigned short port;
					sf::IpAddress sender;
					std::size_t received = 0;

					status = socket.receive(buffer, sizeof(buffer), received, sender, port);

					for(auto i = 0u; i < received; ++i) {
						data.push_back(buffer[i]);
					}
				} while(status == sf::UdpSocket::Partial);

				if(status == sf::UdpSocket::Done)
					ptr->m_onDataReceived(data);
				else if(status != sf::UdpSocket::NotReady)
					ptr->m_onError(status);
			}
		}
	};

	std::vector<UdpSocket*> UdpSocket::sockets;
}

#endif // UDPSOCKET_H
