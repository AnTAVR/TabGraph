/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/19 19:36:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Window.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "parser/BMP.hpp"
#include "parser/GLSL.hpp"

Material::Material(const std::string &name)
{
	set_name(name);
	if ((shader = Shader::get_by_name("shader_default")) == nullptr) {
		shader = GLSL::parse("shader_default", Engine::program_path() + "./res/shaders/empty.glsl", ForwardShader);
	}
	if ((depth_shader = Shader::get_by_name("default_depth")) == nullptr) {
		depth_shader = GLSL::parse("default_depth", Engine::program_path() + "./res/shaders/depth.vert", Engine::program_path() + "./res/shaders/depth.frag");
	}
}

Material	*Material::create(const std::string &name)
{
	auto	mtl = new Material(name);
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
	while ((m = Engine::material(i)) != nullptr)
	{
		if (h == m->id()) {
			return (m);
		}
		i++;
	}
	return (nullptr);
}

void	Material::bind_textures()
{
	shader->bind_texture("Texture.Albedo", texture_albedo, GL_TEXTURE1);
	shader->set_uniform("Texture.Use_Albedo", texture_albedo != nullptr);
	shader->bind_texture("Texture.Specular", texture_specular, GL_TEXTURE2);
	shader->set_uniform("Texture.Use_Specular", texture_specular != nullptr ? true : false);
	shader->bind_texture("Texture.Roughness", texture_roughness, GL_TEXTURE3);
	shader->set_uniform("Texture.Use_Roughness", texture_roughness != nullptr ? true : false);
	shader->bind_texture("Texture.Metallic", texture_metallic, GL_TEXTURE4);
	shader->set_uniform("Texture.Use_Metallic", texture_metallic != nullptr ? true : false);
	shader->bind_texture("Texture.Emitting", texture_emitting, GL_TEXTURE5);
	shader->set_uniform("Texture.Use_Emitting", texture_emitting != nullptr ? true : false);
	shader->bind_texture("Texture.Normal", texture_normal, GL_TEXTURE6);
	shader->set_uniform("Texture.Use_Normal", texture_normal != nullptr ? true : false);
	shader->bind_texture("Texture.Height", texture_height, GL_TEXTURE7);
	shader->set_uniform("Texture.Use_Height", texture_height != nullptr ? true : false);
	shader->bind_texture("Texture.AO", texture_ao, GL_TEXTURE8);
	shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE9);
	shader->bind_texture("Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE10);
}

void	Material::bind_values()
{
	shader->set_uniform("Material.Albedo", albedo);
	shader->set_uniform("Material.Specular", specular);
	shader->set_uniform("Material.Emitting", emitting);
	shader->set_uniform("Material.Roughness", roughness);
	shader->set_uniform("Material.Metallic", metallic);
	shader->set_uniform("Material.Alpha", alpha);
	shader->set_uniform("Material.Parallax", parallax);
	shader->set_uniform("Material.Ior", ior);
	shader->set_uniform("Texture.Scale", uv_scale);
}
