/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TabManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:13:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:45:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <TabNode.hpp>
#include <TabTime.hpp>
#include <TabInput.hpp>

namespace sf{
	class RenderWindow;
}


namespace TabGraph 
{
	class Window;
	class Manager
	{
		public:
			Manager();
			~Manager();
			static void		CreateWindow(const int &, const int &, const std::string &);
			static float	GetDeltaTime();
		private:
			Window		*_window;
	};

}
