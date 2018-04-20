/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TabNode.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:09:53 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:32:18 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

namespace TabGraph {
	class Node
	{
	public:
		Node();
		~Node();
		void	Release();
		int		GetRef() const;
		void	AddRef();
		void	AddChild(Node * const);
		void	RemoveChild(Node * const);
	protected:
		int					_references;
		std::vector<Node *> _children;
	};
}
