/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 17:24:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/23 11:37:45 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"

Events	*g_events = nullptr;

Events::Events()
{
	memset(_kcallbacks, 0, sizeof(_kcallbacks));
	_rcallback = nullptr;
}

Events	&Events::_get()
{
	if (!g_events)
		g_events = new Events();
	return (*g_events);
}

void	Events::set_key_callback(SDL_Scancode key, t_kcallback callback)
{
	_get()._kcallbacks[key] = callback;
}

void	Events::set_refresh_callback(t_kcallback callback)
{
	_get()._rcallback = callback;
}


void	Events::window(SDL_Event *event)
{
	if (event->window.event == SDL_WINDOWEVENT_CLOSE)
		Engine::stop();
	else if (event->window.event == SDL_WINDOWEVENT_RESIZED)
		Window::resize();
}

void	Events::keyboard(SDL_Event *event)
{
	if (_get()._kcallbacks[event->key.keysym.scancode])
		_get()._kcallbacks[event->key.keysym.scancode](event);
}

int		Events::filter(void *, SDL_Event *event)
{
	if (event->type == SDL_QUIT)
		Engine::stop();
	else if (event->type == SDL_WINDOWEVENT)
		Events::window(event);
	else if (event->type == SDL_KEYUP
		|| event->type == SDL_KEYDOWN)
		Events::keyboard(event);
	return (0);
}

int		Events::refresh(void)
{
	if (_get()._rcallback)
		_get()._rcallback(nullptr);
	return (0);
}
