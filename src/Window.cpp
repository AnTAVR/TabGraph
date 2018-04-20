/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abartz <abartz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 22:07:18 by abartz            #+#    #+#             */
/*   Updated: 2018/04/20 22:07:18 by abartz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//#include "SFML/Graphics.hpp"
#include "Window.hpp"

namespace TabGraph
{
	Window::Window(const int &width, const int &height, const std::string &name)
		: _window(nullptr)
	{
		_window = new sf::RenderWindow(sf::VideoMode(width, height), name);
	}

	Window::~Window()
	{
		if (_window)
			delete _window;
	}
}
