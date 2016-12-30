#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <functional>

namespace sfcb {
	template<typename window_t>
	class Window;

	template<typename window_t, typename event_t>
	class Callback {
	private:
		std::function<void(Window<window_t>&, event_t)> m_func;

	public:
		template<typename T, class ... Ts>
		Callback(T func, const Ts&... args) {
			m_func = [func, args...](Window<window_t>& win, event_t ev) {
				func(win, ev, args...);
			};
		}

		Callback()
		{ }

		inline void operator()(Window<window_t>& window, event_t ev) {
			this->m_func(window, ev);
		}
	};
}

#endif // CALLBACK_HPP
