/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/20 01:43:44 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "parser/BMP.hpp"
#include "parser/GLSL.hpp"

Texture	*Material::_texture_brdf = nullptr;

Material::Material(const std::string &name) : shader(nullptr),
	albedo(new_vec3(0.5, 0.5, 0.5)), uv_scale(new_vec2(1, 1)), alpha(1), texture_albedo(nullptr)
{
	set_name(name);
	if (_texture_brdf == nullptr)
	{
		_texture_brdf = BMP::parse("brdf", "./res/brdfLUT.bmp");
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	if ((shader = Shader::get_by_name("default")) == nullptr) {
		shader = GLSL::parse("default", "./res/shaders/default.vert", "./res/shaders/default.frag");
	}
}

void	Material::set_name(const std::string &name)
{
	_name = name;
	std::hash<std::string> hash_fn;
	_id = hash_fn(name);
}

const std::string	&Material::name()
{
	return (_name);
}

Material	*Material::create(const std::string &name)
{
	auto	mtl = new Material(name);
	mtl->shader = Shader::get_by_name("default");
	Engine::add(*mtl);
	return (mtl);
}

Material	*Material::get_by_name(const std::string &name)
{
	auto		i = 0;
	size_t		h;
	Material	*m;
	
	std::hash<std::string> hash_fn;
	h = hash_fn(name);
	while ((m = Engine::material(i)))
	{
		if (h == m->_id) {
			return (m);
		}
		i++;
	}
	return (nullptr);
}

void	Material::bind_textures()
{
	shader->bind_texture("in_Texture_Albedo", texture_albedo, GL_TEXTURE1);
	shader->set_uniform("in_Use_Texture_Albedo", texture_albedo != nullptr ? true : false);
	shader->bind_texture("in_Texture_Env", Engine::current_environment()->diffuse, GL_TEXTURE11);
	shader->bind_texture("in_Texture_Env_Spec", Engine::current_environment()->brdf, GL_TEXTURE12);
	shader->bind_texture("in_Texture_BRDF", _texture_brdf, GL_TEXTURE13);

}

void	Material::bind_values()
{
	shader->set_uniform("in_Albedo", albedo);
	shader->set_uniform("in_UVScale", uv_scale);
	shader->set_uniform("in_Alpha", alpha);
}

void	Material::load_textures()
{
	_texture_brdf->load();
	if (texture_albedo != nullptr) {
		texture_albedo->load();
	}
}
