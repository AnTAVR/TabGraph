/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/20 16:30:23 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/20 19:41:46 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <TabManager.hpp>
#include <unistd.h>
#include <string>

int main()
{
	TabGraph::Manager::CreateWindow(1280, 720, std::string("Test"));
	sleep(500);
	return 0;
}