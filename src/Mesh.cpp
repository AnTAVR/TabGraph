/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/26 18:00:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AABB.hpp"
#include "Vgroup.hpp"
#include <algorithm>

std::vector<std::shared_ptr<Mesh>>	Mesh::_meshes;

Mesh::Mesh(const std::string &name) : Renderable(name)
{
	bounding_element = new AABB;
}



std::shared_ptr<Mesh>	Mesh::create(const std::string &name) /*static*/
{
	auto	m = std::shared_ptr<Mesh>(new Mesh(name));
	Mesh::add(m);
	Renderable::add(m);
	Node::add(m);
	return (m);
}

std::shared_ptr<Mesh>	Mesh::get_by_name(const std::string &name) /*static*/
{
	std::hash<std::string>	hash_fn;
	auto					h = hash_fn(name);
	for (auto n : _meshes) {
		if (h == n->id())
			return (n);
	}
	return (nullptr);
}

std::shared_ptr<Mesh>	Mesh::get(unsigned index) /*static*/
{
	if (index >= _meshes.size())
		return (nullptr);
	return (_meshes.at(index));
}

void	Mesh::add(std::shared_ptr<Mesh> mesh) /*static*/
{
	_meshes.push_back(mesh);
}

std::shared_ptr<Vgroup>	Mesh::vgroup(unsigned index)
{
	if (index >= _vgroups.size())
		return (nullptr);
	return (_vgroups.at(index));
}

void	Mesh::add(std::shared_ptr<Vgroup> group)
{
	if (nullptr == group)
		return ;
	_vgroups.push_back(group);
}

void	Mesh::load()
{
	if (_is_loaded)
		return ;
	for (auto vg : _vgroups) {
		vg->load();
	}
}

#include <iostream>

bool	Mesh::render_depth(RenderMod mod)
{
	bool	ret = false;
	auto	mvp = mat4_combine(Camera::current()->projection(), Camera::current()->view(), transform());
	auto	normal_matrix = mat4_transpose(mat4_inverse(transform()));
	
	load();
	std::shared_ptr<Shader>	last_shader;
	for (auto vg : _vgroups) {
		auto	vgMaterial = vg->material();
		if (vgMaterial == nullptr)
			continue ;
		auto	depthShader = vgMaterial->depth_shader();
		depthShader->use();
		if (last_shader != depthShader) {
			depthShader->set_uniform("Matrix.Model", transform());
			depthShader->set_uniform("Matrix.ModelViewProjection", mvp);
			depthShader->set_uniform("Matrix.Normal", normal_matrix);
		}
		//depthShader->use(false);
		if (vg->render_depth(mod))
			ret = true;
		last_shader = depthShader;
	}
	return (ret);
}

bool	Mesh::render(RenderMod mod)
{
	bool	ret = false;
	auto	mvp = mat4_combine(Camera::current()->projection(), Camera::current()->view(), transform());
	auto	normal_matrix = mat4_transpose(mat4_inverse(transform()));

	load();
	std::shared_ptr<Shader>	last_shader;
	for (auto vg : _vgroups) {
		auto	vgMaterial = vg->material();
		if (vgMaterial == nullptr)
			continue ;
		auto	vgShader = vgMaterial->shader();
		vgShader->use();
		if (last_shader != vgShader) {
			vgShader->set_uniform("Matrix.Model", transform());
			vgShader->set_uniform("Matrix.ModelViewProjection", mvp);
			vgShader->set_uniform("Matrix.Normal", normal_matrix);
		}
		//vg->material->shader->use(false);
		if (vg->render(mod))
			ret = true;
		last_shader = vgShader;
	}
	return (ret);
}

void	Mesh::set_cull_mod(GLenum mod)
{
	_cull_mod = mod;
}

/*void	Mesh::sort(renderable_compare compare)
{
	std::sort(_vgroups.begin(), _vgroups.end(), compare);
}*/

void	Mesh::center()
{
	for (auto vg : _vgroups) {
		vg->center(bounding_element->center);
	}
	bounding_element->min = vec3_sub(bounding_element->min, bounding_element->center);
	bounding_element->max = vec3_sub(bounding_element->max, bounding_element->center);
	bounding_element->center = new_vec3(0, 0, 0);
}
