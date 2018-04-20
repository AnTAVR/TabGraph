/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TabInput.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:16:46 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 18:14:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace TabGraph {
	class Input
	{
	public:
		Input();
		~Input();
		float			GetAxis(std::string axis);
	};
}