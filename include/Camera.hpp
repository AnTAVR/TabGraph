/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abartz <abartz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/21 16:47:42 by abartz            #+#    #+#             */
/*   Updated: 2018/04/23 15:03:04 by abartz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

namespace TabGraph
{
	class Camera : Node
	{
	public:
		Camera();
		virtual ~Camera();

		virtual void Update() final;

	private:
		glm::vec3	_look_at;
		glm::vec3	_direction;
		glm::vec3	_up;
		glm::vec3	_right;
		glm::vec3	_up_left;
	};
}
