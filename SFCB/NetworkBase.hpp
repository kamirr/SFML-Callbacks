#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <SFML/Network/Socket.hpp>
#include <SFML/Config.hpp>
#include <vector>

namespace sfcb {
	typedef sf::Socket::Status SocketStatus;
	typedef std::vector<sf::Int8> buffer_t;

	class TcpSocket;

	struct SocketEvent {
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

		SocketEvent(const buffer_t& buffer) {
			this->type = Type::DataReceived;
			this->buffer = &buffer;
		}
		SocketEvent(SocketStatus status) {
			this->type = Type::Error;
			this->status = status;
		}
		SocketEvent(TcpSocket& socket) {
			this->type = Type::Connected;
			this->client = &socket;
		}
	};
}

#endif // NETWORK_HPP
