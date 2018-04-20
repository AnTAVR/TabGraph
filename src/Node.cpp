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
	Node::Node() : _references(0)
	{
		AddRef();
	}
	Node::~Node()
	{
		Release();
	}
	void	Node::Release()
	{
		if (_references > 0)
			--_references;
	}
	int		Node::GetRef() const
	{
		return (_references);
	}
	void	Node::AddRef()
	{
		++_references;
	}
	void	Node::AddChild(Node *const node)
	{
		if (!node)
			return ;
		node->AddRef();
		_children.push_back(node);
	}
	void	Node::RemoveChild(Node *const node)
	{
		auto found = std::find(_children.begin(), _children.end(), node);
		if (found == _children.end())
			return ;
		std::remove(_children.begin(), _children.end(), node);
		node->Release();
	}
}
