/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TabWindow.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:22:12 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:25:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

namespace TabGraph {
	class Window
	{
	public:
		Window (const int &, const int &, const std::string &);
		~Window();
	private:
		Window();
		sf::RenderWindow *_window;
	};
}