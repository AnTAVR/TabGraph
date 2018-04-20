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

#include "Manager.hpp"

namespace TabGraph
{
	Manager tabGraph;

	Manager::Manager() : _window(nullptr)
	{}

	Manager::~Manager()
	{
		if (_window)
			delete _window;
	}

	void	Manager::CreateWindow(
						const int& x, const int& y,
						const std::string& title)
	{
		tabGraph._window = new Window(x, y, title);
	}

	float	Manager::GetDeltaTime()
	{
		return (tabGraph._deltaTime);
	}
}
