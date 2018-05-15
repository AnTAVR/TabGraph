/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderable.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/15 21:07:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

class Material;
class AABB;

/*
** Because Renderable is an interface, it is abstract and should not be instanciated.
*/

class	Renderable : public Node
{
public:
	virtual void	render() = 0;
	virtual void	load() = 0;
	virtual void	bind() = 0;
	static void		alpha_sort();
	bool			is_loaded() { return (_is_loaded); };
	static Renderable	*get_by_name(const std::string &);
    Material		*material;
protected:
	bool		_is_loaded;
	Renderable(const std::string &name) : Node(name), material(nullptr), _is_loaded(false) {};
};
