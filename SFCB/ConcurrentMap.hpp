#ifndef CONCURRENTMAP_HPP
#define CONCURRENTMAP_HPP

#include <condition_variable>
#include <unordered_map>
#include <mutex>

template <typename key_t, typename value_t>
class ConcurrentMap {
public:
	value_t get(const key_t& key) {
		std::unique_lock<std::mutex> mlock(m_mutex);
		const value_t* ptr;

		try {
			 ptr = &m_map.at(key);
		} catch(std::out_of_range&) {
			throw std::out_of_range("Invalid access to ConcurrentMap – key does not exist.");
		}

		mlock.unlock();
		m_cond.notify_all();

		return *ptr;
	}

	void set(const key_t& key, const value_t& value) {
		std::unique_lock<std::mutex> mlock(m_mutex);
		m_map[key] = value;
		mlock.unlock();
		m_cond.notify_all();
	}

	template<typename ... Ts>
	void emplace(const key_t& key, const Ts& ... args) {
		this->set(key, value_t(args ...));
	}

	bool hasKey(const key_t& key) {
		return (m_map.find(key) != m_map.end());
	}

private:
	std::mutex m_mutex;
	std::condition_variable m_cond;
	std::unordered_map<key_t, value_t> m_map;
};

#endif // CONCURRENTMAP_HPP
