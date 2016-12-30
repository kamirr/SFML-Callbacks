#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <functional>
#include <algorithm>
#include <vector>

#include "Callback.hpp"

namespace sfcb {
	class TcpSocket
	: public sf::NonCopyable {
	private:
		std::function<void(const std::vector<sf::Int8>&)> m_onDataReceived;
		std::function<void(sf::Socket::Status)> m_onError;
		std::function<void(TcpSocket&)> m_onConnected;

		static std::vector<TcpSocket*> sockets;
		sf::TcpSocket m_socket;

		bool m_connecting = false;

	public:
		TcpSocket() {
			m_socket.setBlocking(false);
			sockets.push_back(this);
		}
		~TcpSocket() {
			auto it = std::find(sockets.begin(), sockets.end(), this);
			sockets.erase(it);
		}

		sf::IpAddress getRemoteAddress() const {
			return this->m_socket.getRemoteAddress();
		}

		unsigned short getRemotePort() const {
			return this->m_socket.getRemotePort();
		}

		unsigned short getLocalPort() const {
			return this->m_socket.getLocalPort();
		}

		void connect(const sf::IpAddress& remoteAddress, const unsigned short remotePort, sf::Time timeout = sf::Time::Zero) {
			auto status = this->m_socket.connect(remoteAddress, remotePort, timeout);

			this->m_connecting = true;

			if(status != sf::Socket::Done
			&& status != sf::Socket::NotReady)
				this->m_onError(status);
		}

		void disconnect() {
			this->m_socket.disconnect();
		}

		size_t send(const std::vector<sf::Int8>& buffer) {
			size_t sent;
			auto status = this->m_socket.send(buffer.data(), buffer.size(), sent);

			if(status != sf::Socket::Done) {
				this->m_onError(status);
			}

			return sent;
		}

		template<typename func_t, typename ... args_t>
		void onDataReceived(func_t func, const args_t& ... args) {
			this->m_onDataReceived = Callback<const std::vector<sf::Int8>&>(func, args ...);
		}

		template<typename func_t, typename ... args_t>
		void onError(func_t func, const args_t& ... args) {
			this->m_onError = Callback<sf::Socket::Status>(func, args ...);
		}

		template<typename func_t, typename ... args_t>
		void onConnected(func_t func, const args_t& ... args) {
			this->m_onConnected = Callback<TcpSocket&>(func, args ...);
		}

		static void handleCallbacks() {
			for(auto* ptr: sockets) {
				auto& socket = ptr->m_socket;

				std::vector<sf::Int8> data;
				sf::Socket::Status status;

				do {
					char buffer[1024];
					std::size_t received = 0;

					status = socket.receive(buffer, sizeof(buffer), received);

					for(auto i = 0u; i < received; ++i) {
						data.push_back(buffer[i]);
					}
				} while(status == sf::Socket::Partial);

				if(status == sf::Socket::Done) {
					ptr->m_onDataReceived(data);
				} else if(status != sf::Socket::NotReady) {
					ptr->m_onError(status);
				}

				if(ptr->m_connecting && socket.getRemotePort()) {
					ptr->m_connecting = false;
					ptr->m_onConnected(*ptr);
				}
			}
		}
	};

	std::vector<TcpSocket*> TcpSocket::sockets;
}

#endif // TCPSOCKET_HPP
