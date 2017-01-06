#ifndef RESOURCELOADER_HPP
#define RESOURCELOADER_HPP

#include <SFML/System/MemoryInputStream.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <iterator>
#include <fstream>

#include "SFCB/ConcurrentQueue.hpp"
#include "SFCB/ConcurrentMap.hpp"
#include "SFCB/Callback.hpp"
#include "SFCB/Base.hpp"

namespace sfcb {
	struct ResourceRequest {
		std::string path;
		Callback<cbuffer_t&> callback;
	};

	class ResourceLoader
		: public sf::NonCopyable {
	private:
		ConcurrentMap<size_t, std::string> currentlyProcessing;
		ConcurrentMap<std::string, buffer_t> data;

		ConcurrentQueue<ResourceRequest> m_requests;
		std::vector<std::thread> m_threads;
		bool m_asyncMode = true;

		void worker(size_t id) {
			while (true) {
				requestBuffer_impl(this->m_requests.pop(), id);
			}
		}

		void requestBuffer_impl(ResourceRequest req, size_t id) {
			if(data.hasKey(req.path)) {
				req.callback(data.get(req.path));
				return;
			}

			for(auto i = 0u; i < std::thread::hardware_concurrency(); ++i) {
				if(currentlyProcessing.get(i) == req.path) {
					m_requests.push(req);
					return;
				}
			}

			currentlyProcessing.set(id, req.path);

			std::ifstream is(req.path);
			is >> std::noskipws;

			std::istream_iterator<char> start(is), end;
			data.emplace(req.path, start, end);

			currentlyProcessing.set(id, "{none}");

			req.callback(data.get(req.path));
		}

	protected:
		ResourceLoader() {
			if(std::thread::hardware_concurrency() == 1) {
				this->enableAsync(false);
			}

			for(auto i = 0u; i < std::thread::hardware_concurrency(); ++i) {
				this->m_threads.emplace_back(&ResourceLoader::worker, this, i);
				currentlyProcessing.set(i, "{none}");
			}
		}

	public:
		static ResourceLoader& getInstance() {
			static ResourceLoader res;
			return res;
		}

		void requestBuffer(const std::string& path, Callback<cbuffer_t&> callback) {
			if(this->m_asyncMode) {
				this->m_requests.push({path, callback});
			} else {
				this->requestBuffer_impl({path, callback}, 0);
			}
		}

		void requestStream(const std::string& path, Callback<sf::MemoryInputStream&> callback) {
			this->requestBuffer(path, [callback](cbuffer_t& buffer) {
				sf::MemoryInputStream stream;
				stream.open(buffer.data(), buffer.size());

				callback(stream);
			});
		}

		void completeRemainingTasks() {
			while(this->m_requests.size())
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		void enableAsync(bool async) {
			if(this->m_requests.size()) {
				throw std::logic_error("Can't change async mode with remaining unfinished tasks");
			}

			this->m_asyncMode = async;
		}
	};
}

#endif // RESOURCELOADER_HPP
