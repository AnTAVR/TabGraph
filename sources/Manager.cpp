/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 19:23:45 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:45:52 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <thread>
#include "Manager.hpp"

namespace TabGraph
{
	Manager handler;

	/* ********************************* */

	Manager::Manager() {}
	Manager::~Manager() {}

	void	Manager::CreateWindow(
						const int& w, const int& h,
						const std::string& t)
	{
		handler._window = new sf::RenderWindow(sf::VideoMode(w, h), t);
		handler._window()->setFramerateLimit(60);
	}

	/*
	** http://www.cplusplus.com/reference/chrono/duration/count/
	*/
	double	Manager::GetDeltaTime()
	{
		return handler._deltaTime().count();
	}

	/* ********************************* */

	inline void	Manager::UpdateEvents(sf::Event& ev)
	{
		while (handler._window()->pollEvent(ev))
		{
			if (ev.type == sf::Event::Closed)
				handler._window()->close();
		}
	}

	/*
	** http://www.cplusplus.com/reference/chrono/high_resolution_clock/now/
	*/
	inline void	Manager::UpdateDeltaTime(time_point<steady_clock>& tic)
	{
		handler._deltaTime = duration_cast<duration<double, std::micro>>(
			high_resolution_clock::now() - tic);
		// ligne necessaire ?
		//std::this_thread::sleep_for(handler._deltaTime());
	}

	/* ********************************* */

	void	Manager::Run()
	{
		sf::Event ev;
		time_point<steady_clock> tic;

		while (handler._window()->isOpen())
		{
			tic = high_resolution_clock::now();
			UpdateEvents(ev);

			// some things
			UpdateDeltaTime(tic);
		}
	}
}
