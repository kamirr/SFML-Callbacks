// CallbackContext.hpp

#ifndef CALLBACKCONTEXT_HPP
#define CALLBACKCONTEXT_HPP

#include <SFML/Config.hpp>

namespace sfcb {
	/*
	 * Callback context specifies set of callbacks that are called in given situation,
	 * that lets programmer add a few callbacks to one event and chose which one should be called
	 */

	class CallbackContext {
		const sf::Int32 m_UID;

	public:
		CallbackContext(const sf::Int32 UID)
			: m_UID { UID }
		{ }

		sf::Int32 uid() const {
			return this->m_UID;
		}
	};
}

#endif // CALLBACKCONTEXT_HPP
