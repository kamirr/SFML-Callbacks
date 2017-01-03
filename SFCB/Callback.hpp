#ifndef CALLBACK_HPP
#define CALLBACK_HPP

#include <functional>

namespace sfcb {
	template<typename ... callback_args_t>
	class Callback {
	private:
		std::function<void(callback_args_t ...)> m_func;

	public:
		template<typename func_t, typename ... args_t>
		void set(func_t func, const args_t&... args) {
			m_func = [func, args ...](callback_args_t ... callback_args) {
				func(callback_args ..., args ...);
			};
		}

		template<typename func_t, typename ... args_t>
		Callback(func_t func, const args_t&... args) {
			this->set(func, args ...);
		}

		Callback()
		{
			m_func = [](callback_args_t ...) { };
		}

		inline void operator()(const callback_args_t ... callback_args) const {
			this->m_func(callback_args ...);
		}
	};
}

#endif // CALLBACK_HPP
