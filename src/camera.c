/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:30:02 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/10 16:28:35 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int		camera_get_target_index(int camera_index)
{
	t_camera	*camera;

	camera = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!camera)
		return (-1);
	return (camera->target_index);
}

int		camera_create(float fov)
{
	t_camera	camera;

	memset(&camera, 0, sizeof(t_camera));
	camera.fov = fov;
	camera.target_index = -1;
	ezarray_push(&engine_get()->cameras, &camera);
	return (engine_get()->cameras.length - 1);
}

void	camera_orbite(int camera_index, float phi, float theta, float radius)
{
	VEC3		target_position;
	VEC3		new_position;
	t_transform	*target;

	target = ezarray_get_index(engine_get()->transforms,
		camera_get_target_index(camera_index));
	if (target)
		target_position = target->position;
	else
		target_position = new_vec3(0, 0, 0);
	new_position.x = target_position.x + radius * sin(phi) * cos(theta);
	new_position.z = target_position.z + radius * sin(phi) * sin(theta);
	new_position.y = target_position.y + radius * cos(phi);
	camera_set_position(camera_index, new_position);
}

void	camera_update(int camera_index)
{
	t_transform	*ct;
	VEC3		target;
	t_camera	*c;
	VEC2		size;

	c = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!c)
		return ;
	ct = ezarray_get_index(engine_get()->transforms, c->target_index);
	target = ct ? ct->position : new_vec3(0, 0, 0);
	c->view = mat4_lookat(c->position, target, UP);
	size = window_get_size();
	c->projection = mat4_perspective(c->fov,
		size.x / size.y, 0.1, 1000);
}
