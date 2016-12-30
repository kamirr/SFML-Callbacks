// CallbackWindow.hpp

#ifndef SFMLCALLBACKS_H
#define SFMLCALLBACKS_H

#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <exception>
#include <atomic>
#include <vector>
#include <map>

#include "Context.hpp"
#include "Callback.hpp"

namespace sfcb {
	typedef sf::Event::EventType EventType;

	template<typename window_t = sf::Window>
	class Window
		: public window_t
	{
	private:
		typedef std::pair<EventType, sf::Int32> key_t;
		typedef WindowCallback<window_t, sf::Event> callback_t;

		std::map<key_t, callback_t> m_callbacks;

		sf::Int32 m_currentContext = -2;
		sf::Int32 m_contextCount = 0;

	protected:
		/* Make pollEvent protected */
		using window_t::pollEvent;

	public:
		using window_t::window_t;

		static callback_t getEmptyCallback() {
			return callback_t([](window_t&, sf::Event){ });
		}

		Context createCallbackContext() {
			return Context(++m_contextCount);
		}

		Context getUniversalCallbackContext() {
			return Context(-1);
		}

		void setCurrentContext(Context c) {
			if(c.uid() == -1)
				throw std::invalid_argument("Can't set current callback context to universal one");

			this->m_currentContext = c.uid();
		}

		template<typename T, class ... Ts>
		void setCallback(EventType type, Context context, T func, const Ts&... args) {
			key_t key(type, context.uid());
			m_callbacks[key] = callback_t(func, args ...);
		}

		void handleCallbacks() {
			for(sf::Event ev; this->pollEvent(ev);) {
				for(const auto& pair: m_callbacks) {
					key_t key = pair.first;
					callback_t call = pair.second;

					bool key_event_t_match = (key.first == ev.type);
					bool key_context_match = (key.second == -1 || key.second == this->m_currentContext);

					if(key_event_t_match && key_context_match)
						call(*this, ev);
				}
			}
		}
	};
}

#endif // SFMLCALLBACKS_H
