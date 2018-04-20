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

#include "Node.hpp"
#include "Window.hpp"

namespace TabGraph
{
	class Manager
	{
		public:
			Manager();
			~Manager();
			static void		CreateWindow(
								const int& x, const int& y,
								const std::string& title);
			static float	GetDeltaTime();
			static float	GetTime();

		private:
			void	UpdateDeltaTime();
			void	UpdateNodes();
			Window	*_window;
			float	_deltaTime;
			float	_time;
	};
}
