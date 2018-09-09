/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:52:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/09 23:16:16 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

Shader::Shader(const std::string &name) : _program(0), _in_use(false)
{
	set_name(name);
}

Shader	*Shader::create(const std::string &name)
{
	auto	shader = new Shader(name);
	Engine::add(*shader);
	return (shader);
}

ShaderVariable	*Shader::get_uniform(const std::string &name)
{
	auto	it = _uniforms.find(name);
	if (it != _uniforms.end()) {
		return (&it->second);
	}
	return (nullptr);
}

void			Shader::set_uniform(const std::string &name, const int &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform1iv(v->loc, nbr, &value);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const bool &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	int	val = value;
	glUniform1iv(v->loc, nbr, &val);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const unsigned &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform1uiv(v->loc, nbr, &value);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const float &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform1fv(v->loc, nbr, &value);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const VEC2 &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform2fv(v->loc, nbr, &value.x);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const VEC3 &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniform3fv(v->loc, nbr, &value.x);
	if (!bound) {
		use(false);
	}
}

void			Shader::set_uniform(const std::string &name, const MAT4 &value, unsigned nbr)
{
	auto	v = get_uniform(name);
	if (v == nullptr) {
		return ;
	}
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glUniformMatrix4fv(v->loc, nbr, GL_FALSE, (float*)&value);
	if (!bound) {
		use(false);
	}
}

bool			Shader::in_use()
{
	return (_in_use);
}

void			Shader::use(const bool &use_program)
{
	if (!use_program) {
		glUseProgram(0);
		return ;
	}
	glUseProgram(_program);
	_in_use = true;
}

Shader		*Shader::get_by_name(const std::string &name)
{
	int			i;
	size_t			h;
	Shader	*s;

	i = 0;
	std::hash<std::string>	hash_fn;
	h = hash_fn(name);
	while ((s = Engine::shader(i)) != nullptr)
	{
		if (h == s->id()) {
			return (s);
		}
		i++;
	}
	return (nullptr);
}

void	Shader::unbind_texture(GLenum texture_unit)
{
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP);
	if (!bound) {
		use(false);
	}
}

void	Shader::bind_texture(const std::string &name,
	Texture *texture, const GLenum &texture_unit)
{
	bool	bound = in_use();
	if (!bound) {
		use();
	}
	if (texture == nullptr) {
		unbind_texture(texture_unit);
	}
	else {
		texture->load();
		glActiveTexture(texture_unit);
		glBindTexture(texture->target(), texture->glid());
	}
	set_uniform(name, int(texture_unit - GL_TEXTURE0));
	if (!bound) {
		use(false);
	}
}