/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TabWindow.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:23:43 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:27:12 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <SFML/Graphics.hpp>
#include <TabWindow.hpp>

namespace TabGraph {
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