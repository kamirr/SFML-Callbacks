#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <SFML/Network/Socket.hpp>
#include <SFML/Config.hpp>
#include <vector>

namespace sfcb {
	typedef sf::Socket::Status SocketStatus;
	typedef std::vector<sf::Int8> buffer_t;
	typedef const buffer_t cbuffer_t;

	class TcpSocket;

	struct SocketEvent {
		enum Type {
			DataReceived,
			Connected,
			Error
		} type;

		union {
			SocketStatus status;
			TcpSocket* client;
			cbuffer_t* buffer;
		};

		SocketEvent(cbuffer_t& buffer) {
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
