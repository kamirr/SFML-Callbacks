#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <SFML/Network/Socket.hpp>
#include <SFML/Config.hpp>
#include <vector>

namespace sfcb {
	typedef sf::Socket::Status SocketStatus;
	typedef std::vector<sf::Int8> buffer_t;
}

#endif // NETWORK_HPP
