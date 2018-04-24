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

#include <memory>

namespace TabGraph
{
	template <typename T>
	class Property
	{
	public:
		virtual ~Property() {}
		virtual T& operator= (const T& f) { return value = f; }
		virtual T const & operator() (void) const { return value; }

	protected:
		T value;
	};

	template <typename T>
	class HeapProperty
	{
	public:
		virtual ~HeapProperty() {}
		virtual std::unique_ptr<T>& operator= (T* const f)
		{
			value.reset(f);
			return value;
		}
		virtual T* const operator() (void) const
		{
			return value.get();
		}

	protected:
		std::unique_ptr<T> value;
	};
}
