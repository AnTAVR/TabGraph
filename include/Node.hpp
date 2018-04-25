/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:09:53 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:32:18 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLObjs.hpp"
#include "Property.hpp"

namespace TabGraph
{
	class Node
	{
	public:
		Node();
		virtual ~Node();

		using SharedNode = std::shared_ptr<Node>;

		void	AddChild(SharedNode& node);
		void	RemoveChild(SharedNode& node);

		virtual void	Update() = 0;


	protected:
		glm::vec3	_position;
		glm::mat4x4	_mvp;

		std::vector<SharedNode> _children;
	};
}
