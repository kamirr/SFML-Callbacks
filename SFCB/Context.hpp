// CallbackContext.hpp

#ifndef CALLBACKCONTEXT_HPP
#define CALLBACKCONTEXT_HPP

#include <SFML/Config.hpp>

namespace sfcb {
	/*
	 * Callback context specifies set of callbacks that are called in given situation,
	 * that lets programmer add a few callbacks to one event and chose which one should be called
	 */

	class Context {
		const sf::Int32 m_UID;

	public:
		Context(const sf::Int32 UID)
			: m_UID { UID }
		{ }

		sf::Int32 uid() const {
			return this->m_UID;
		}
	};
}

#endif // CALLBACKCONTEXT_HPP
