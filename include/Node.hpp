/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:09:53 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/21 15:37:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

namespace TabGraph
{
	class Node
	{
	public:
		Node();
		virtual ~Node();
		void	Release();
		int		GetRef() const;
		void	AddRef();
		void	AddChild(Node* const);
		void	RemoveChild(Node* const);

		virtual void	Update() = 0;

	protected:
		/*
		** Au lieu d'utiliser une variable int,
		** il faudrait reflechir a l'utilisation de shared_ptr :
		** http://en.cppreference.com/w/cpp/memory/shared_ptr
		** c'est + mieux o/
		** https://stackoverflow.com/questions/106508/what-is-a-smart-pointer-and-when-should-i-use-one
		*/
		int					_references;
		std::vector<Node*> _children;
	};
}
