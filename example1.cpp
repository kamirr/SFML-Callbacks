// main.cpp

#include "SFCB/Window.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <vector>

/* Namespace with callbacks */
namespace callbacks {
	void onClose(sf::RenderWindow& window, sf::Event) {
		std::cout << "Y u do dis? ;-;" << std::endl;
		window.close();
	}

	void onMouseMoved1(sf::RenderWindow& window, sf::Event ev) {
		window.clear({
			static_cast<sf::Uint8>(double(ev.mouseMove.x) / 300. * 235. + 20),
			20,
			20});
	}

	void onMouseMoved2(sf::RenderWindow& window, sf::Event ev) {
		window.clear({
			20,
			static_cast<sf::Uint8>(double(ev.mouseMove.x) / 300. * 235. + 20),
			20});
	}
}

struct KeyLogger {
	std::vector<sf::Keyboard::Key> keys;

	void onKeyPressed(sfcb::Window<sf::RenderWindow>& window, sf::Event ev, std::vector<sfcb::Context> contexts) {
		std::cout << "Key pressed, SFML code: " << ev.key.code << std::endl;
		this->keys.push_back(ev.key.code);

		if(ev.key.code <= sf::Keyboard::Z) {
			if(size_t(ev.key.code) >= contexts.size()) {
				std::cout << "No such context (" << ev.key.code + 1 << ")!" << std::endl;
			} else {
				window.setCurrentContext(contexts[size_t(ev.key.code)]);
				std::cout << "Current callback context: " << ev.key.code + 1 << std::endl;
			}
		}
	}
};

int main()
{
	KeyLogger logger;

	/* Create window, acts just like window given in parameter */
	sfcb::Window<sf::RenderWindow> app({300, 300}, "app");
	app.setVerticalSyncEnabled(true);

	/* Get universal context and create 2 new ones */
	auto universal = app.getUniversalCallbackContext();
	std::vector<sfcb::Context> contexts;
	for(auto i = 0; i < 2; ++i)
		contexts.push_back(app.createCallbackContext());

	/* Set current context for callbacks */
	app.setCurrentContext(contexts[0]);

	/* Connect a few callbacks to some events */
	app.setCallback(sf::Event::Closed, universal, callbacks::onClose);
	app.setCallback(sf::Event::MouseMoved, contexts[0], callbacks::onMouseMoved1);
	app.setCallback(sf::Event::MouseMoved, contexts[1], callbacks::onMouseMoved2);

	/* Connect callback in class with additional parameters */
	auto wrapper = [&logger](sfcb::Window<sf::RenderWindow>& window, sf::Event ev, std::vector<sfcb::Context> contexts) {
		logger.onKeyPressed(window, ev, contexts);
	};
	sfcb::Callback<sfcb::Window<sf::RenderWindow>&, sf::Event> callback(wrapper, contexts);

	app.setCallback(sf::Event::KeyPressed, universal, callback);

	/* Minimal main loop */
	app.clear({20, 20, 20});
	while(app.isOpen()) {
		/* Apply logic */
		app.handleCallbacks();

		/* All callbacks are called right before displaying window */
		app.display();
	}

	return 0;
}
