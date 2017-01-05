#ifndef RESOURCELOADER_HPP
#define RESOURCELOADER_HPP

#include <SFML/System/MemoryInputStream.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <iterator>
#include <fstream>

#include "SFCB/ConcurrentQueue.hpp"
#include "SFCB/Callback.hpp"
#include "SFCB/Base.hpp"

namespace sfcb {
	class ResourceLoader
		: public sf::NonCopyable {
	public:
		struct Request {
			std::string path;
			Callback<cbuffer_t&> callback;
		};

	private:
		ConcurrentQueue<Request> m_requests;
		std::vector<std::thread> m_threads;
		bool m_asyncMode = true;

		void worker() {
			while (true) {
				requestBuffer_impl(this->m_requests.pop());
			}
		}

		static void requestBuffer_default_impl(Request req) {
			std::ifstream is(req.path);
			is >> std::noskipws;

			std::istream_iterator<char> start(is), end;
			buffer_t buffer(start, end);

			req.callback(buffer);
		}

		static std::function<void(Request)> requestBuffer_impl;

	protected:
		ResourceLoader() {
			for(auto i = 0u; i < std::thread::hardware_concurrency(); ++i) {
				this->m_threads.emplace_back(&ResourceLoader::worker, this);
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
				this->requestBuffer_impl({path, callback});
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

		void setDataLoadingFunc(std::function<void(Request)> func) {
			this->requestBuffer_impl = func;
		}
	};

	std::function<void(ResourceLoader::Request)> ResourceLoader::requestBuffer_impl = &ResourceLoader::requestBuffer_default_impl;
}

#endif // RESOURCELOADER_HPP
