/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abartz <abartz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 19:23:45 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/01 18:16:20 by abartz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SFML/Window/Event.hpp"
#include "Manager.hpp"

namespace TabGraph
{
	Manager handler;

	/* ********************************* */

	Manager::Manager() {}
	Manager::~Manager() {}

	void	Manager::Init(const int& w, const int& h, const std::string& t)
	{
		handler._window = new sf::Window(sf::VideoMode(w, h), t);
		handler._window()->setFramerateLimit(60);
	}

	/*inline float	Manager::GetDeltaTime()
	{
		return handler._clock.getElapsedTime().asSeconds();
	}*/

	/* ********************************* */

	void	Manager::UpdateNodes()
	{}

	inline void	Manager::UpdateEvents()
	{
		sf::Event ev;
		while (handler._window()->pollEvent(ev))
		{
			if (ev.type == sf::Event::Closed ||
				(ev.type == sf::Event::KeyReleased &&
					ev.key.code == sf::Keyboard::Escape))
			{
				handler._window()->close();
			}
			else if (ev.type == sf::Event::Resized)
			{
				glViewport(0, 0, ev.size.width, ev.size.height);
			}
		}
	}

	/* ********************************* */

	void	Manager::Run()
	{
		glDrawBuffer(GL_BACK);
		glClearColor(0.128f, 0.128f, 0.128f, 1.f);
		glEnable(GL_DEPTH_TEST);

		while (handler._window()->isOpen())
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			UpdateEvents();
			/* *************** */

			// ...

			/* *************** */
			handler._window()->display();
		}
	}
}
