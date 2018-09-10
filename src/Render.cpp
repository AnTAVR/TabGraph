/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:42:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/10 19:42:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/InternalTools.hpp"
#include "Render.hpp"
#include "Engine.hpp"
#include "Light.hpp"
#include "Renderable.hpp"
#include "Camera.hpp"
#include "Cubemap.hpp"
#include "Framebuffer.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include <vector>

/*
** quad is a singleton
*/

const VertexArray	*Render::display_quad()
{
	static VertexArray	*vao = nullptr;
	if (vao != nullptr) {
		return (vao);
	}
	vao = VertexArray::create(4, GL_TRIANGLE_STRIP);
	std::vector<float>	quad(8);
	quad.at(0) = -1.0f;
	quad.at(1) = -1.0f;
	quad.at(2) = 1.0f;
	quad.at(3) = -1.0f;
	quad.at(4) = -1.0f;
	quad.at(5) = 1.0f;
	quad.at(6) = 1.0f;
	quad.at(7) = 1.0f;
	vao->add_buffer(GL_FLOAT, 2, quad);
	return (vao);
}

#include "parser/GLSL.hpp"

Framebuffer	*create_render_buffer(const std::string &name, const VEC2 &size, Shader *shader)
{
	auto	buffer = Framebuffer::create(name, size, shader, 0, 1);
	buffer->create_attachement(GL_RGBA, GL_RGBA8); // Albedo;
	buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
	buffer->create_attachement(GL_RGB, GL_RGB8); // Fresnel;
	buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Material_Values -> Roughness, Metallic, Ior
	buffer->create_attachement(GL_RED, GL_R8); //AO
	buffer->create_attachement(GL_RGB, GL_RGB16_SNORM); // Normal;
	buffer->setup_attachements();
	return (buffer);
}

Framebuffer	*create_back_buffer(const std::string &name, const VEC2 &size, Shader *shader)
{
	auto	buffer = Framebuffer::create(name, size, shader, 0, 1);
	buffer->create_attachement(GL_RGBA, GL_RGBA8); // Color;
	buffer->create_attachement(GL_RGB, GL_R11F_G11F_B10F); // Emitting;
	buffer->setup_attachements();
	return (buffer);
}

#include <parser/BMP.hpp>

void	present(Framebuffer *back_buffer)
{
	static auto	presentShader = GLSL::parse("present",
		Engine::program_path() + "./res/shaders/passthrough.vert", Engine::program_path() + "./res/shaders/present.frag");

	// GENERATE BLOOM FROM out_Brightness
	back_buffer->attachement(1)->blur(BLOOMPASS, 3.5);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	Framebuffer::bind_default();
	presentShader->use();
	presentShader->bind_texture("in_Texture_Color", back_buffer->attachement(0), GL_TEXTURE0);
	presentShader->bind_texture("in_Texture_Emitting", back_buffer->attachement(1), GL_TEXTURE1);
	presentShader->bind_texture("in_Texture_Depth", back_buffer->depth(), GL_TEXTURE2);
	Render::display_quad()->draw();
	presentShader->use(false);
	Window::swap();
}

void	Render::update()
{
	auto	InvViewMatrix = mat4_inverse(Engine::current_camera()->view);
	auto	InvProjMatrix = mat4_inverse(Engine::current_camera()->projection);
	auto	res = Window::internal_resolution();
	auto	node_index = 0;

	while (auto shader = Engine::shader(node_index))
	{
		shader->use();
		shader->set_uniform("Camera.Position", Engine::current_camera()->position());
		shader->set_uniform("Camera.Matrix.View", Engine::current_camera()->view);
		shader->set_uniform("Camera.Matrix.Projection", Engine::current_camera()->projection);
		shader->set_uniform("Camera.InvMatrix.View", InvViewMatrix);
		shader->set_uniform("Camera.InvMatrix.Projection", InvProjMatrix);
		shader->set_uniform("Resolution", new_vec3(res.x, res.y, res.x / res.y));
		shader->set_uniform("Time", SDL_GetTicks() / 1000.f);
		shader->use(false);
		node_index++;
	}
}

struct t_Light
{
	VEC3	Position;
	VEC3	Color;
};

std::vector<t_Light> create_random_lights(unsigned i)
{
	std::vector<t_Light> v;
	for (auto index = 0u; index < i; index++)
	{
		t_Light light;
		light.Position = new_vec3(
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * 10.0,
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * 10.0,
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * 10.0);
		light.Color = new_vec3(
			std::rand() / float(RAND_MAX) / float(i * 0.25),
			std::rand() / float(RAND_MAX) / float(i * 0.25),
			std::rand() / float(RAND_MAX) / float(i * 0.25));
		v.push_back(light);
	}
	return (v);
}

void	Render::scene()
{
	static Texture	*brdf = nullptr;
	if (brdf == nullptr)
	{
		brdf = BMP::parse("brdf", Engine::program_path() + "./res/brdfLUT.bmp");
		brdf->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		brdf->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	static auto	temp_buffer = create_render_buffer("temp_buffer", Window::internal_resolution(), nullptr);
	static auto	temp_buffer1 = create_render_buffer("temp_buffer1", Window::internal_resolution(), nullptr);
	static auto	back_buffer = create_back_buffer("back_buffer", Window::internal_resolution(), nullptr);
	static auto	back_buffer1 = create_back_buffer("back_buffer1", Window::internal_resolution(), nullptr);
	static auto	back_buffer2 = create_back_buffer("back_buffer1", Window::internal_resolution(), nullptr);
	static auto	lighting_shader = GLSL::parse("lighting", Engine::program_path() + "./res/shaders/lighting.frag", LightingShader);
	static auto	elighting_shader = GLSL::parse("lighting_env", Engine::program_path() + "./res/shaders/lighting_env.frag", LightingShader);
	static auto	telighting_shader = GLSL::parse("lighting_env_transparent", Engine::program_path() + "./res/shaders/lighting_env.frag", LightingShader, "#define TRANSPARENT\n");
	static auto	refraction_shader = GLSL::parse("refraction", Engine::program_path() + "./res/shaders/refraction.frag", LightingShader);

	temp_buffer->resize(Window::internal_resolution());
	temp_buffer1->resize(Window::internal_resolution());
	back_buffer->resize(Window::internal_resolution());
	back_buffer1->resize(Window::internal_resolution());
	back_buffer2->resize(Window::internal_resolution());

	/*
	** TODO :
	** 	- CLEANUP CODE
	** 	- MANAGE TRANSPARENT OBJECTS
	*/
	glClearColor(0, 0, 0, 0);
	temp_buffer1->bind();
	glClear(Window::clear_mask());
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	Renderable *node;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderOpaque);
	}

	auto	current_tbuffer = temp_buffer;
	auto	current_tbuffertex = temp_buffer1;

	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	for (Shader *shader : post_treatments)
	{
		// APPLY POST-TREATMENT
		current_tbuffer->bind();
		shader->use();
		shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
		shader->bind_texture("Texture.Emitting",		current_tbuffertex->attachement(1), GL_TEXTURE1);
		shader->bind_texture("Texture.Specular",		current_tbuffertex->attachement(2), GL_TEXTURE2);
		shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
		shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
		shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
		shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
		shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
		shader->bind_texture("Texture.Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE8);
		shader->bind_texture("Texture.Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE9);
		Render::display_quad()->draw();
		shader->use(false);

		auto	temp = current_tbuffer;
		current_tbuffer = current_tbuffertex;
		current_tbuffertex = temp;
	}

	current_tbuffertex->attachement(4)->blur(1, 0.75);

	back_buffer1->bind();
	glClear(GL_COLOR_BUFFER_BIT);

	//static auto	Lights = create_random_lights(200);

	auto	current_backBuffer = back_buffer;
	auto	current_backTexture = back_buffer1;
	lighting_shader->use();
	for (auto i = 0u; Engine::light(i) != nullptr;)
	{
		current_backBuffer->bind();
		for (auto shaderIndex = 0; shaderIndex < 32 && Engine::light(i) != nullptr; shaderIndex++) {
			auto	light = Engine::light(i);
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Position", light->position());
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Color", light->color);
			i++;
		}
		lighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
		lighting_shader->bind_texture("Texture.Emitting",		current_tbuffertex->attachement(1), GL_TEXTURE1);
		lighting_shader->bind_texture("Texture.Specular",		current_tbuffertex->attachement(2), GL_TEXTURE2);
		lighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
		lighting_shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
		lighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
		lighting_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
		lighting_shader->bind_texture("Texture.BRDF",			brdf, GL_TEXTURE7);
		lighting_shader->bind_texture("Texture.Environment.Diffuse",	Engine::current_environment()->diffuse, GL_TEXTURE8);
		lighting_shader->bind_texture("Texture.Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE9);
		lighting_shader->bind_texture("Texture.Back.Color",				current_backTexture->attachement(0), GL_TEXTURE10);
		lighting_shader->bind_texture("Texture.Back.Emitting",			current_backTexture->attachement(1), GL_TEXTURE11);
		lighting_shader->bind_texture("Texture.Back.Depth",				current_backTexture->depth(), GL_TEXTURE12);
		Render::display_quad()->draw();
		auto	buf = current_backTexture;
		current_backTexture = current_backBuffer;
		current_backBuffer = buf;
	}
	lighting_shader->use(false);

	// APPLY LIGHTING SHADER
	// OUTPUT : out_Color, out_Brightness
	current_backBuffer->bind();
	elighting_shader->use();

	elighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
	elighting_shader->bind_texture("Texture.Emitting",			current_tbuffertex->attachement(1), GL_TEXTURE1);
	elighting_shader->bind_texture("Texture.Specular",			current_tbuffertex->attachement(2), GL_TEXTURE2);
	elighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
	elighting_shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
	elighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
	elighting_shader->bind_texture("Texture.Depth",				current_tbuffertex->depth(), GL_TEXTURE6);
	elighting_shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
	elighting_shader->bind_texture("Texture.Environment.Diffuse",		Engine::current_environment()->diffuse, GL_TEXTURE8);
	elighting_shader->bind_texture("Texture.Environment.Irradiance",	Engine::current_environment()->irradiance, GL_TEXTURE9);
	elighting_shader->bind_texture("Texture.Back.Color",				current_backTexture->attachement(0), GL_TEXTURE10);
	elighting_shader->bind_texture("Texture.Back.Emitting",				current_backTexture->attachement(1), GL_TEXTURE11);
	elighting_shader->bind_texture("Texture.Back.Depth",				current_backTexture->depth(), GL_TEXTURE12);
	Render::display_quad()->draw();
	elighting_shader->use(false);

{
	auto	buf = current_backTexture;
	current_backTexture = current_backBuffer;
	current_backBuffer = buf;
}

	//present(current_backTexture);

	// ATTEMPT RENDERING TRANSPARENT OBJECTS
	// WRITE DEPTH FOR FUTUR USE
	current_tbuffer->bind();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	bool	rendered_stuff = false;
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		if (node->render_depth(RenderTransparent))
			rendered_stuff = true;
	}
	
	if (!rendered_stuff) {
		// NO OBJECTS WERE RENDERED PRESENT IMEDIATLY
		present(current_backTexture);
		return ;
	}

	// REWRITE TRANSPARENT OBJECTS
	// WRITE ONLY CLOSEST OBJECTS
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_EQUAL);
	for (auto index = 0; (node = Engine::renderable(index)) != nullptr; index++) {
		node->render(RenderTransparent);
	}

	auto	opaqueBackBuffer = current_backTexture;
	opaqueBackBuffer->attachement(0)->generate_mipmap();
	opaqueBackBuffer->attachement(1)->generate_mipmap();

	current_backTexture = back_buffer2;
	back_buffer2->bind();
	glClear(GL_COLOR_BUFFER_BIT);

{
	auto	temp = current_tbuffer;
	current_tbuffer = current_tbuffertex;
	current_tbuffertex = temp;
}

	glDepthFunc(GL_ALWAYS);
	glDisable(GL_CULL_FACE);
	lighting_shader->use();
	for (auto i = 0u; Engine::light(i) != nullptr;)
	{
		current_backBuffer->bind();
		for (auto shaderIndex = 0; shaderIndex < 32 && Engine::light(i) != nullptr; shaderIndex++) {
			auto	light = Engine::light(i);
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Position", light->position());
			lighting_shader->set_uniform("Light[" + std::to_string(shaderIndex) + "].Color", light->color);
			i++;
		}
		lighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
		lighting_shader->bind_texture("Texture.Emitting",		current_tbuffertex->attachement(1), GL_TEXTURE1);
		lighting_shader->bind_texture("Texture.Specular",		current_tbuffertex->attachement(2), GL_TEXTURE2);
		lighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
		lighting_shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
		lighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
		lighting_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
		lighting_shader->bind_texture("Texture.BRDF",			brdf, GL_TEXTURE7);
		lighting_shader->bind_texture("Texture.Environment.Diffuse",	Engine::current_environment()->diffuse, GL_TEXTURE8);
		lighting_shader->bind_texture("Texture.Environment.Irradiance", Engine::current_environment()->irradiance, GL_TEXTURE9);
		lighting_shader->bind_texture("Texture.Back.Color",				current_backTexture->attachement(0), GL_TEXTURE10);
		lighting_shader->bind_texture("Texture.Back.Emitting",			current_backTexture->attachement(1), GL_TEXTURE11);
		lighting_shader->bind_texture("Texture.Back.Depth",				current_backTexture->depth(), GL_TEXTURE12);
		Render::display_quad()->draw();

		auto	buf = current_backTexture;
		current_backTexture = current_backBuffer;
		current_backBuffer = buf;
	}
	lighting_shader->use(false);

	current_backBuffer->bind();
	telighting_shader->use();
	telighting_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
	telighting_shader->bind_texture("Texture.Emitting",			current_tbuffertex->attachement(1), GL_TEXTURE1);
	telighting_shader->bind_texture("Texture.Specular",			current_tbuffertex->attachement(2), GL_TEXTURE2);
	telighting_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE3);
	telighting_shader->bind_texture("Texture.AO",				current_tbuffertex->attachement(4), GL_TEXTURE4);
	telighting_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE5);
	telighting_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE6);
	telighting_shader->bind_texture("Texture.BRDF",				brdf, GL_TEXTURE7);
	telighting_shader->bind_texture("Texture.Environment.Diffuse",		Engine::current_environment()->diffuse, GL_TEXTURE8);
	telighting_shader->bind_texture("Texture.Environment.Irradiance",	Engine::current_environment()->irradiance, GL_TEXTURE9);
	telighting_shader->bind_texture("Texture.Back.Color",				current_backTexture->attachement(0), GL_TEXTURE10);
	telighting_shader->bind_texture("Texture.Back.Emitting",			current_backTexture->attachement(1), GL_TEXTURE11);
	telighting_shader->bind_texture("Texture.Back.Depth",				current_backTexture->depth(), GL_TEXTURE12);

	Render::display_quad()->draw();
	telighting_shader->use(false);

{
	auto	buf = current_backTexture;
	current_backTexture = current_backBuffer;
	current_backBuffer = buf;
}

	refraction_shader->use();
	current_backBuffer->bind();
	refraction_shader->bind_texture("Texture.Albedo",			current_tbuffertex->attachement(0), GL_TEXTURE0);
	refraction_shader->bind_texture("Texture.Specular",			current_tbuffertex->attachement(2), GL_TEXTURE1);
	refraction_shader->bind_texture("Texture.MaterialValues",	current_tbuffertex->attachement(3), GL_TEXTURE2);
	refraction_shader->bind_texture("Texture.Normal",			current_tbuffertex->attachement(5), GL_TEXTURE3);
	refraction_shader->bind_texture("Texture.Depth",			current_tbuffertex->depth(), GL_TEXTURE4);
	refraction_shader->bind_texture("Texture.Back.Color",		current_backTexture->attachement(0), GL_TEXTURE5);
	refraction_shader->bind_texture("Texture.Back.Emitting",	current_backTexture->attachement(1), GL_TEXTURE6);
	refraction_shader->bind_texture("opaqueBackColor",		opaqueBackBuffer->attachement(0), GL_TEXTURE7);
	refraction_shader->bind_texture("opaqueBackEmitting",	opaqueBackBuffer->attachement(1), GL_TEXTURE8);
	Render::display_quad()->draw();
	refraction_shader->use(false);

	present(current_backBuffer);
}

void	Render::add_post_treatment(Shader *shader)
{
	if (shader != nullptr)
		post_treatments.insert(shader);
}

void	Render::add_post_treatment(const std::string &name, const std::string &path)
{
	auto shader = GLSL::parse(name, path, PostShader);

	if (shader != nullptr)
		post_treatments.insert(shader);
}

void	Render::remove_post_treatment(Shader *shader)
{
	if (shader != nullptr)
		post_treatments.erase(shader);
}
