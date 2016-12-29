#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <SFML/Window/Event.hpp>
#include <functional>

namespace sfcb {
	template<typename window_t>
	class CallbackWindow;

	template<typename window_t, typename event_t>
	class Callback {
	private:
		std::function<void(CallbackWindow<window_t>&, event_t)> m_func;

	public:
		template<typename T, class ... Ts>
		Callback(T func, const Ts&... args) {
			m_func = std::bind(
				func,
				std::placeholders::_1,
				std::placeholders::_2,
				args ...
			);
		}

		Callback()
		{ }

		inline void operator()(CallbackWindow<window_t>& window, event_t ev) {
			this->m_func(window, ev);
		}
	};
}

#endif // CALLBACK_HPP
