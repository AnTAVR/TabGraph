/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scope.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/23 01:30:13 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCOPE_H
# define SCOPE_H

# include <GL/glew.h>
# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
# include <stdint.h>
# include <ezmem.h>
# include <vml.h>
# include <libft.h>
# include <stdio.h>

# define WIDTH		1280
# define HEIGHT		720
# define IWIDTH		1920
# define IHEIGHT	1080
# define SHADOWRES	2048
# define UP			(VEC3){0, 1, 0}
# define ULL		unsigned long long
# define ANISOTROPY	16.f
# define MSAA		8
# define BLOOMPASS	2

typedef struct	s_framebuffer
{
	GLuint		id;
	ARRAY		color_attachements;
	int			depth;
	int			shader;
	VEC2		size;
}				t_framebuffer;

typedef struct	s_point
{
	VEC3		color;
	float		power;
	float		attenuation;
	float		falloff;
}				t_point;

typedef t_point	t_spot;

typedef struct	s_dir
{
	VEC3		color;
	float		power;
}				t_dir;

typedef union	u_light_data
{
	t_point		point;
	t_spot		spot;
	t_dir		directional;
}				t_light_data;

typedef struct	s_light
{
	int				transform_index;
	int8_t			type;
	int8_t			cast_shadow;
	t_light_data	data;
	int				render_buffer;
}				t_light;

typedef struct	s_mtl
{
	VEC3		albedo;
	VEC3		emitting;
	VEC2		uv_scale;
	float		roughness;
	float		metallic;
	float		refraction;
	float		alpha;
	float		parallax;
	float		stupidity;
	int			texture_albedo;
	int			texture_roughness;
	int			texture_metallic;
	int			texture_emitting;
	int			texture_normal;
	int			texture_height;
	int			texture_stupid;
}				t_mtl;

typedef struct	s_texture
{
	ULL			id;
	GLuint		id_ogl;
	STRING		name;
	int			width;
	int			height;
	char		bpp;
	GLenum		target;
	UCHAR		*data;
	BOOL		loaded;
}				t_texture;

typedef struct	s_shadervariable
{
	ULL			id;
	STRING		name;
	GLint		size;
	GLenum		type;
	GLuint		loc;
}				t_shadervariable;

typedef struct	s_shader
{
	ULL			id;
	STRING		name;
	GLuint		program;
	ARRAY		uniforms;
	ARRAY		attributes;
}				t_shader;

typedef struct	s_material
{
	ULL			id;
	STRING		name;
	t_mtl		data;
	int			shader_index;
	int			in_campos;
	int			in_transform;
	int			in_modelmatrix;
	int			in_normalmatrix;
	int			in_albedo;
	int			in_emitting;
	int			in_uvmin;
	int			in_uvmax;
	int			in_roughness;
	int			in_metallic;
	int			in_refraction;
	int			in_alpha;
	int			in_parallax;
	int			in_stupidity;
	int			in_use_texture_albedo_alpha;
	int			in_texture_albedo;
	int			in_use_texture_albedo;
	int			in_texture_roughness;
	int			in_use_texture_roughness;
	int			in_texture_metallic;
	int			in_use_texture_metallic;
	int			in_texture_emitting;
	int			in_use_texture_emitting;
	int			in_texture_normal;
	int			in_use_texture_normal;
	int			in_texture_height;
	int			in_use_texture_height;
	int			in_texture_env;
	int			in_texture_env_spec;
	int			in_texture_stupid;
	int			in_texture_shadow;
	int			in_shadowtransform;
}				t_material;

typedef struct	s_vgroup
{
	ULL			mtl_id;
	int			mtl_index;
	ARRAY		v;
	ARRAY		vn;
	ARRAY		vt;
	VEC2		uvmax;
	VEC2		uvmin;
	GLuint		v_arrayid;
	GLuint		v_bufferid;
	GLuint		vn_bufferid;
	GLuint		vt_bufferid;
}				t_vgroup;

typedef struct	s_aabb
{
	VEC3		min;
	VEC3		max;
	VEC3		center;
}				t_aabb;

typedef struct	s_mesh
{
	int			transform_index;
	t_aabb		bounding_box;
	ARRAY		vgroups;
}				t_mesh;

typedef struct	s_camera
{
	VEC3		position;
	MAT4		view;
	MAT4		projection;
	FRUSTUM		frustum;
	float		fov;
	int			target_index;
}				t_camera;

typedef struct	s_window
{
	SDL_Window		*sdl_window;
	SDL_GLContext	gl_context;
	VEC4			clear_color;
	GLbitfield		clear_mask;
	int				render_buffer;
}				t_window;

typedef void(*kcallback)(SDL_Event *event);

typedef struct	s_engine
{
	int8_t		loop;
	int8_t		swap_interval;
	t_window	*window;
	ARRAY		cameras;
	ARRAY		meshes;
	ARRAY		lights;
	ARRAY		transforms;
	ARRAY		materials;
	ARRAY		shaders;
	ARRAY		textures;
	ARRAY		textures_env;
	ARRAY		framebuffers;
	int			env;
	int			env_spec;
	float		delta_time;
	float		stupidity;
	float		new_stupidity;
	kcallback	kcallbacks[285];
	char		*program_path;
}				t_engine;

//extern t_engine	*engine_get();

/*
** Engine functions
*/

t_engine	*engine_get();
void		engine_load_env();
void		engine_set_key_callback(SDL_Scancode keycode, kcallback callback);

/*
** Camera functions
*/
int			camera_get_target_index(int camera_index);
int			camera_create(float fov);
void		camera_set_target(int camera_index, int transform_index);
void		camera_set_position(int camera_index, VEC3 position);
void		camera_orbite(int camera_index, float phi, float theta, float radius);
void		camera_update(int camera_index);

/*
** Material functions
*/
t_material	new_material(char *name);
int			material_get_index_by_name(char *name);
int			material_get_index_by_id(ULL h);
void		material_assign_shader(int material_index, int shader_index);
void		material_set_textures(int material_index);
void		material_set_uniforms(int material_index);
void		material_load_textures(int material_index);

/*
** Mesh functions
*/
void		mesh_rotate(int mesh_index, VEC3 rotation);
void		mesh_scale(int mesh_index, VEC3 scale);
void		mesh_translate(int mesh_index, VEC3 position);
void		mesh_load(int mesh_index);
void		mesh_center(int mesh_index);
void		mesh_render(int camera_index, int mesh_index);

/*
** Vgroup functions
*/

void		vgroup_load(int mesh_index, int vgroup_index);
void		vgroup_render(int camera_index, int mesh_index, int vgroup_index);
void		vgroup_center(int mesh_index, int vgroup_index);

/*
** Texture functions
*/

GLuint		texture_get_ogl_id(int texture_index);
int			texture_get_by_name(char *name);
int			texture_create(VEC2 size, GLenum target, GLenum internal_format, GLenum format);
void		texture_set_parameters(int texture_index, int parameter_nbr, GLenum *parameters, GLenum *values);
void		texture_assign(int texture_index, int dest_texture_index, GLenum target);
void		texture_load(int texture_index);
void		texture_generate_mipmap(int texture_index);

/*
** Parser tools
*/

char		**split_path(const char *path);
ULL			hash(unsigned char *str);
char		*convert_backslash(char *str);

/*
** .mtllib parser
*/
int			load_mtllib(char *path);

/*
** .obj parser
*/

int			load_obj(char *path);
VEC3		parse_vec3(char **split);
VEC2		parse_vec2(char **split);

/*
** .bmp parser
*/

int			load_bmp(const char *imagepath);

/*
** Shader functions
*/

void		shader_set_texture(int shader_index, int uniform_index, int texture_index, GLenum texture_unit);
void		shader_set_uniform(int shader_index, int uniform_index, void *value);
void		shader_use(int shader_index);
int			shader_get_uniform_index(int shader_index, char *name);
int			shader_get_by_name(char *name);
int			load_shaders(const char *name, const char *vertex_file_path,const char *fragment_file_path);
int			transform_create(VEC3 position, VEC3 rotation, VEC3 scale);

/*
** Framebuffer functions
*/

void			framebuffer_bind(int framebuffer);
int				framebuffer_get_attachement(int framebuffer, int color_attachement);
int				framebuffer_get_depth(int framebuffer);
int				framebuffer_get_shader(int framebuffer);
int				framebuffer_create(VEC2 size, int shader, int color_attachements, int depth);

#endif