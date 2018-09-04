/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/04 11:19:27 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "Framebuffer.hpp"
#include "Shader.hpp"
#include "Window.hpp"
#include "Render.hpp"
#include "parser/GLSL.hpp"

Texture::Texture(const std::string &name) :
_glid(0),
_size(new_vec2(0, 0)),
_bpp(0),
_data_size(0),
_data_format(GL_UNSIGNED_BYTE),
_target(0),
_format(0),
_internal_format(0),
_data(nullptr),
_loaded(false)
{
	set_name(name);
}

Texture::Texture(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format, void *data) : Texture(name)
{
	_target = target;
	_format = f;
	_internal_format = fi;
	_data_format = data_format;
	_data_size = get_data_size(data_format);
	_data = static_cast<GLubyte*>(data);
	_bpp = get_bpp(f, data_format);
	_size = s;
	glGenTextures(1, &_glid);
	if (values_per_pixel() < 4) {
		set_parameteri(GL_TEXTURE_SWIZZLE_A, GL_ONE);
	}
	set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	set_parameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, _glid, -1, name.c_str());
#endif //GL_DEBUG
}

Texture		*Texture::create(const std::string &name, VEC2 s, GLenum target, GLenum f, GLenum fi, GLenum data_format, void *data)
{
	Texture	*t;

	t = new Texture(name, s, target, f, fi, data_format, data);
	Engine::add(*t);
	return (t);
}

size_t	Texture::get_bpp(GLenum texture_format, GLenum data_format)
{
	auto	data_size = Texture::get_data_size(data_format);
	switch (texture_format)
	{
		case GL_RED :
		return (1 * 8 * data_size);
		case GL_RGB :
		case GL_BGR :
		return (3 * 8 * data_size);
		case GL_RGBA :
		case GL_BGRA :
		return (4 * 8 * data_size);
		default : return (0);
	}
}

size_t	Texture::get_data_size(GLenum data_format)
{
	switch (data_format)
	{
		case GL_FLOAT :
		case GL_FIXED :
		case GL_INT :
		case GL_UNSIGNED_INT :
		return (sizeof(GLfloat));
		case GL_BYTE :
		case GL_UNSIGNED_BYTE :
		return (sizeof(GLubyte));
		case GL_HALF_FLOAT :
		case GL_SHORT :
		case GL_UNSIGNED_SHORT :
		return (sizeof(GLushort));
		case GL_DOUBLE :
		return (sizeof(GLdouble));
		default : return (0);
	}
}

GLenum	Texture::target() const
{
	return (_target);
}

VEC2	Texture::size() const
{
	return (_size);
}

void	*Texture::data() const
{
	return (_data);
}

void	Texture::assign(Texture &dest_texture, GLenum target)
{
	//if (!_loaded)
	//	load();
	glBindTexture(_target, _glid);
	glBindTexture(dest_texture._target, dest_texture._glid);
	glTexImage2D(target, 0, dest_texture._internal_format, dest_texture._size.x, dest_texture._size.y, 0,
		dest_texture._format, dest_texture._data_format, dest_texture._data);
	glBindTexture(_target, 0);
	glBindTexture(dest_texture._target, 0);
}


void	Texture::unload()
{
	if (!_loaded) {
		return ;
	}
	glDeleteTextures(1, &_glid);
	_loaded = false;
	_glid = 0u;
}

void	Texture::load()
{
	if (_loaded) {
		return ;
	}
	if (MAXTEXRES != -1 && _data && (_size.x > MAXTEXRES || _size.y > MAXTEXRES)) {
		resize(new_vec2(
			std::min(int(_size.x), MAXTEXRES),
			std::min(int(_size.y), MAXTEXRES)));
	}
	if (_glid == 0u) {
		glGenTextures(1, &_glid);
	}
	glBindTexture(_target, _glid);
	if (_size.x > 0 && _size.y > 0) {
		glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format, _data_format, _data);
	}
	for (auto p : _parametersi) {
		glTexParameteri(_target, p.first, p.second);
	}
	for (auto p : _parametersf) {
		glTexParameterf(_target, p.first, p.second);
	}
	//glGenerateMipmap(_target);
	generate_mipmap();
	glBindTexture(_target, 0);
	_loaded = true;
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, _glid, -1, name().c_str());
#endif //GL_DEBUG
}

void	Texture::generate_mipmap()
{
	set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(_target, _glid);
	glGenerateMipmap(_target);
	glBindTexture(_target, 0);
}

void		Texture::format(GLenum *format, GLenum *internal_format)
{
	*format = _format;
	*internal_format = _internal_format;
}

GLubyte		Texture::bpp() const
{
	return (_bpp);
}

GLuint		Texture::glid() const
{
	return (_glid);
}

Texture		*Texture::get_by_name(const std::string &name)
{
	int			i;
	size_t			h;
	Texture	*t;

	i = 0;
	std::hash<std::string>	hash_fn;
	h = hash_fn(name);
	while ((t = Engine::texture(i)) != nullptr)
	{
		if (h == t->id()) {
			return (t);
		}
		i++;
	}
	return (nullptr);
}

GLenum	Texture::data_format()
{
	return (_data_format);
}

size_t	Texture::data_size()
{
	return (_data_size);
}

size_t	Texture::values_per_pixel()
{
	return (_data_size ? _bpp / _data_size / 8 : 0);
}

GLubyte	*Texture::texelfetch(const VEC2 &uv)
{
	int				opp;

	if (_data == nullptr) {
		return (nullptr);
	}
	auto	nuv = uv;
	nuv.x = CLAMP(nuv.x, 0, 1);
	nuv.y = CLAMP(nuv.y, 0, 1);
	nuv = new_vec2(
		round((_size.x - 1) * nuv.x),
		round((_size.y - 1) * nuv.y));
	opp = _bpp / 8;
	return (&_data[int(_size.x * nuv.y  + nuv.x) * opp]);
}

void	Texture::set_pixel(const VEC2 &uv, const VEC4 value)
{
	int			opp;
	VEC4		val{0, 0, 0, 1};

	opp = _bpp / 8;
	val = value;
	if (_data == nullptr) {
		_data = new GLubyte[int(_size.x * _size.y) * opp];
	}
	GLubyte			*p;
	p = texelfetch(uv);
	auto	valuePtr = reinterpret_cast<float*>(&val);
	for (auto i = 0, j = 0; i < int(opp / _data_size) && j < 4; ++i, ++j)
	{
		if (_data_size == 1)
			p[i] = valuePtr[j] * 255.f;
		else if (_data_size == sizeof(GLfloat))
			static_cast<GLfloat*>((void*)p)[i] = valuePtr[j];
	}
}

void	Texture::set_pixel(const VEC2 &uv, const GLubyte *value)
{
	int			opp;

	opp = _bpp / 8;
	if (_data == nullptr) {
		_data = new GLubyte[int(_size.x * _size.y) * opp];
	}
	GLubyte			*p;
	p = texelfetch(uv);
	for (auto i = 0; i < opp; ++i) {
		p[i] = value[i];
	}
}

void	Texture::set_parameterf(GLenum p, float v)
{
	_parametersf[p] = v;
	glBindTexture(_target, _glid);
	glTexParameterf(_target, p, v);
	glBindTexture(_target, 0);
}

void	Texture::set_parameteri(GLenum p, GLenum v)
{
	_parametersi[p] = v;
	glBindTexture(_target, _glid);
	glTexParameteri(_target, p, v);
	glBindTexture(_target, 0);
}

void	Texture::set_parameters(int p_nbr, GLenum *p, GLenum *v)
{
	glBindTexture(_target, _glid);
	while (p_nbr > 0)
	{
		glTexParameteri(_target, p[p_nbr - 1], v[p_nbr - 1]);
		p_nbr--;
	}
	glBindTexture(_target, 0);
}

VEC4	Texture::sample(const VEC2 &uv)
{
	VEC3		vt[4];
	VEC4		value{0, 0, 0, 1};

	if (_data == nullptr) {
		return (value);
	}
	vt[0] = new_vec3(CLAMP(_size.x * uv.x, 0, _size.x - 1),
		CLAMP(_size.y * uv.y, 0, _size.y - 1), 0);
	auto nuv = new_vec2(fract(vt[0].x), fract(vt[0].y));
	vt[0].z = ((1 - nuv.x) * (1 - nuv.y));
	vt[1] = new_vec3(std::min(_size.x - 1, vt[0].x + 1),
		std::min(_size.y - 1, vt[0].y + 1), (nuv.x * (1 - nuv.y)));
	vt[2] = new_vec3(vt[0].x, vt[1].y, ((1 - nuv.x) * nuv.y));
	vt[3] = new_vec3(vt[1].x, vt[0].y, (nuv.x * nuv.y));
	auto	opp = _bpp / 8;
	for (auto i = 0; i < 4; ++i)
	{
		auto	d = &_data[int(round(vt[i].y) * _size.x + round(vt[i].x)) * opp];
		for (auto j = 0; j < int(opp / _data_size); ++j)
		{
			if (_data_size == 1)
				reinterpret_cast<float*>(&value)[j] += (d[j] * vt[i].z) / 255.f;
			else if (_data_size == sizeof(GLfloat))
				reinterpret_cast<float*>(&value)[j] += static_cast<float*>((void*)d)[j] * vt[i].z;
		}
	}
	return (value);
}

bool	Texture::is_loaded()
{
	return (_loaded);
}

void	Texture::resize(const VEC2 &ns)
{
	GLubyte	*d;

	if (_data != nullptr)
	{
		auto	opp = _bpp / 8;
		d = new GLubyte[unsigned(ns.x * ns.y * opp)];
		for (auto	y = 0; y < ns.y; ++y)
		{
			for (auto	x = 0; x < ns.x; ++x)
			{
				auto	uv = new_vec2(x / ns.x, y / ns.y);
				auto	value = sample(uv);
				auto	p = &d[int(ns.x * y + x) * opp];
				for (auto z = 0; z < int(opp / _data_size); ++z) {
					if (_data_size == 1)
						p[z] = reinterpret_cast<float*>(&value)[z] * 255.f;
					else if (_data_size == sizeof(GLfloat))
						reinterpret_cast<float*>(p)[z] = reinterpret_cast<float*>(&value)[z];
				}
			}
		}
		delete [] _data;
		_data = d;
	}
	_size = ns;
	if (_glid != 0u)
	{
		glDeleteTextures(1, &_glid);
		glGenTextures(1, &_glid);
		glBindTexture(_target, _glid);
		glTexImage2D(_target, 0, _internal_format, _size.x, _size.y, 0, _format, _data_format, _data);
		glBindTexture(_target, 0);
	}
}


Framebuffer	*Texture::_generate_blur_buffer(const std::string &bname)
{
	auto	buffer = Framebuffer::create(bname, size(), nullptr, 0, 0);
	buffer->create_attachement(_format, _internal_format);
	buffer->setup_attachements();
	return (buffer);
}

void	Texture::blur(const int &pass, const float &radius)
{
	if (pass == 0)
		return ;
	if (_blur_buffer0 == nullptr)
		_blur_buffer0 = _generate_blur_buffer("blur0");
	if (_blur_buffer1 == nullptr)
		_blur_buffer1 = _generate_blur_buffer("blur1");
	_blur_buffer0->resize(size());
	_blur_buffer1->resize(size());

	static auto	blurShader = GLSL::parse("blur", Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/blur.frag");
	
	auto	totalPass = pass * 4;
	auto	cbuffer = _blur_buffer0;
	auto	ctexture = this;
	float	angle = 0;
	Texture	*attachement = nullptr;
	blurShader->use();
	while (totalPass > 0)
	{
		VEC2			direction;
		direction = mat2_mult_vec2(mat2_rotation(angle), new_vec2(1, 1));
		direction = vec2_scale(direction, radius);
		if (totalPass == 1) {
			attachement = cbuffer->attachement(0);
			cbuffer->set_attachement(0, this);
		}
		cbuffer->bind();
		blurShader->set_uniform("in_Direction", direction);
		blurShader->bind_texture("in_Texture_Color", ctexture, GL_TEXTURE0);
		Render::display_quad()->draw();
		angle = CYCLE(angle + (M_PI / 4.f), 0, M_PI);
		if (totalPass == 1)
			cbuffer->set_attachement(0, attachement);
		if (totalPass % 2 == 0) {
			cbuffer = _blur_buffer1;
			ctexture = _blur_buffer0->attachement(0);
		}
		else {
			cbuffer = _blur_buffer0;
			ctexture = _blur_buffer1->attachement(0);
		}
		totalPass--;
	}
	blurShader->use(false);
	Framebuffer::bind_default();
}
