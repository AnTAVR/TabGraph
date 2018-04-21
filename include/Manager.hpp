/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Manager.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:13:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:52:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <chrono>
#include "Property.hpp"
//#include "Node.hpp"
#include "SFML/Graphics.hpp"

using namespace std::chrono;

namespace TabGraph
{
	class Manager
	{
	private:
		/*
		** sf::RenderWindow est specifique pour les rendus 2D,
		** autant utilise sf::Window, non ?
		*/
		HeapProperty<sf::RenderWindow>	_window;
		Property<duration<double, std::micro>>	_deltaTime;

		static void	UpdateEvents(sf::Event&);
		static void	UpdateNodes();
		static void	UpdateDeltaTime(time_point<steady_clock>& tic);

	public:
		Manager();
		~Manager();

		static void		CreateWindow(
							const int& width, const int& height,
							const std::string& title);
		static double	GetDeltaTime();
		static void		Run();
	};
}
