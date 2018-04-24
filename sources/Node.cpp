/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 18:58:39 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:35:03 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <algorithm>
#include "Node.hpp"

namespace TabGraph
{
	Node::Node()
	{}

	Node::~Node()
	{}

	void	Node::AddChild(SharedNode& node)
	{
		_children.push_back(node);
	}

	void	Node::RemoveChild(SharedNode& node)
	{
		auto found = std::find(_children.begin(), _children.end(), node);
		if (found != _children.end())
			_children.erase(found);
	}
}
