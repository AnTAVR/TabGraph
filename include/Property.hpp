/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Property.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abartz <abartz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/21 16:47:42 by abartz            #+#    #+#             */
/*   Updated: 2018/05/01 16:08:14 by abartz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>

template <typename T>
class Property
{
public:
	Property() {}
	//Property(Property& p) : value(p()) {}
	//Property(T& v) : value(v) {}
	Property(T&& v) { std::swap(value, v); }
	//virtual ~Property() {}

	virtual T const & operator() (void) const { return value; }

protected:
	T value;
};

template <typename T>
using uPtr = std::unique_ptr<T>;

template <typename T>
using UProperty = Property<uPtr<T>>;

template <typename T>
class SmartProperty : public UProperty<T>
{
public:
	SmartProperty() : UProperty<T>() {}
	SmartProperty(T& v) : UProperty<T>(uPtr<T>(new T(v))) {}
	//virtual ~SmartProperty() {}

	virtual uPtr<T>& operator= (T* const f)
	{
		this->value.reset(f);
		return this->value;
	}
};
