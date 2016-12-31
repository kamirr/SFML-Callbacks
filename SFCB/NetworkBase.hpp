#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <SFML/Network/Socket.hpp>
#include <SFML/Config.hpp>
#include <vector>

namespace sfcb {
	typedef sf::Socket::Status SocketStatus;
	typedef std::vector<sf::Int8> buffer_t;

	class TcpSocket;

	struct TcpEvent {
		enum Type {
			DataReceived,
			Connected,
			Error
		} type;

		union {
			const buffer_t* buffer;
			SocketStatus status;
			TcpSocket* client;
		};

		TcpEvent(const buffer_t& buffer) {
			this->type = Type::DataReceived;
			this->buffer = &buffer;
		}
		TcpEvent(SocketStatus status) {
			this->type = Type::Error;
			this->status = status;
		}
		TcpEvent(TcpSocket& socket) {
			this->type = Type::Connected;
			this->client = &socket;
		}
	};
}

#endif // NETWORK_HPP
