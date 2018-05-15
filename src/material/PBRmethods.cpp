/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PBRmethods.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:17:13 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/15 20:53:36 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Cubemap.hpp"
#include "PBRMaterial.hpp"
#include "parser/GLSL.hpp"

PBRMaterial::PBRMaterial(const std::string &name) : Material(name),
	texture_specular(nullptr),
	texture_roughness(nullptr),
	texture_metallic(nullptr),
	texture_emitting(nullptr),
	texture_normal(nullptr),
	texture_height(nullptr),
	texture_ao(nullptr)
{
	if (!(shader = Shader::get_by_name("defaultPBR")))
		shader = GLSL::parse("defaultPBR", "./res/shaders/defaultPBR.vert", "./res/shaders/defaultPBR.frag");
}

PBRMaterial	*PBRMaterial::create(const std::string &name)
{
	PBRMaterial	*mtl = new PBRMaterial(name);

	mtl->parallax = 0.01;
	mtl->emitting = new_vec3(0, 0, 0);
	mtl->metallic = 0;
	mtl->roughness = 0.5;
	mtl->specular = new_vec3(0.04, 0.04, 0.04);
	Engine::add(*mtl);
	return (mtl);
}

void	PBRMaterial::bind_textures()
{
	Material::bind_textures();
	shader->bind_texture("in_Texture_Specular", texture_specular, GL_TEXTURE2);
	shader->set_uniform("in_Use_Texture_Specular", texture_specular ? true : false);
	shader->bind_texture("in_Texture_Roughness", texture_roughness, GL_TEXTURE3);
	shader->set_uniform("in_Use_Texture_Roughness", texture_roughness ? true : false);
	shader->bind_texture("in_Texture_Metallic", texture_metallic, GL_TEXTURE4);
	shader->set_uniform("in_Use_Texture_Metallic", texture_metallic ? true : false);
	shader->bind_texture("in_Texture_Emitting", texture_emitting, GL_TEXTURE5);
	shader->set_uniform("in_Use_Texture_Emitting", texture_emitting ? true : false);
	shader->bind_texture("in_Texture_Normal", texture_normal, GL_TEXTURE6);
	shader->set_uniform("in_Use_Texture_Normal", texture_normal ? true : false);
	shader->bind_texture("in_Texture_Height", texture_height, GL_TEXTURE7);
	shader->set_uniform("in_Use_Texture_Height", texture_height ? true : false);
	shader->bind_texture("in_Texture_AO", texture_ao, GL_TEXTURE8);
	shader->set_uniform("in_Use_Texture_AO", texture_ao ? true : false);
}

void	PBRMaterial::bind_values()
{
	Material::bind_values();
	shader->set_uniform("in_Specular", specular);
	shader->set_uniform("in_Emitting", emitting);
	shader->set_uniform("in_Roughness", roughness);
	shader->set_uniform("in_Metallic", metallic);
	shader->set_uniform("in_Parallax", parallax);
}

void	PBRMaterial::load_textures()
{
	Material::load_textures();
	if (texture_specular)
		texture_specular->load();
	if (texture_roughness)
		texture_roughness->load();
	if (texture_metallic)
		texture_metallic->load();
	if (texture_emitting)
		texture_emitting->load();
	if (texture_normal)
		texture_normal->load();
	if (texture_height)
		texture_height->load();
	if (texture_ao)
		texture_ao->load();
}
