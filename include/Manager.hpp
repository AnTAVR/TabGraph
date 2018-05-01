/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abartz <abartz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:13:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/01 16:13:31 by abartz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <functional>
#include "SFML/OpenGL.hpp"
#include "SFML/Window/Window.hpp"
#include "Node.hpp"

namespace TabGraph
{
	class Manager
	{
	private:

		SmartProperty<sf::Window>	_window;
		//sf::Clock	_clock;

		std::function<void()>	_keysCallback;
		std::function<void()>	_mouseCallback;
		std::function<void()>	_scrollCallback;

		

		static void	UpdateNodes();
		static void	UpdateEvents();
		//static void	UpdateTitle();

	public:
		Manager();
		~Manager();

		static void	Init(const int& width, const int& height, const std::string& title);
		static void	Run();

		//static float	GetDeltaTime();
	};
}
