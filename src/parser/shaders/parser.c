/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/21 16:37:40 by gpinchon          #+#    #+#             */
/*   Updated: 2017/02/21 18:01:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <scope.h>

GLuint	compile_shader(const char *path, GLenum type)
{
	GLuint	shaderid;
	int		fd;
	char	*buf;

	buf = ft_strjoin(g_program_path, path);
	if (access(buf, F_OK | W_OK)
	|| (fd = open(buf, O_RDONLY)) <= 0)
	{
		free(buf);
		return (0);
	}
	free(buf);
	buf = file_to_str(fd);
	shaderid = glCreateShader(type);
	glShaderSource(shaderid, 1, (const char **)(&buf) , NULL);
	glCompileShader(shaderid);
	free(buf);
	return (shaderid);
}

int		check_shader(GLuint id)
{
	char	*log;
	GLint	result;
	GLint	loglength;

	result = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength > 0)
	{
		log = ft_memalloc(sizeof(char) * loglength);
		glGetShaderInfoLog(id, loglength, NULL, &log[0]);
		ft_putendl(log);
		free(log);
		return (-1);
	}
	return (0);
}

int		check_program(GLuint id)
{
	char	*log;
	GLint	result;
	GLint	loglength;

	result = GL_FALSE;
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglength);
	if (loglength > 0)
	{
		log = ft_memalloc(sizeof(char) * loglength);
		glGetProgramInfoLog(id, loglength, NULL, &log[0]);
		ft_putendl(log);
		free(log);
		return (-1);
	}
	return (0);
}

GLuint		link_shaders(GLuint vertexid, GLuint fragmentid)
{
	GLuint progid = glCreateProgram();

	glAttachShader(progid, vertexid);
	glAttachShader(progid, fragmentid);
	glLinkProgram(progid);
	return(progid);
}

GLuint	load_shaders(const char *vertex_file_path,const char *fragment_file_path)
{	
	GLuint VertexShaderID = compile_shader(vertex_file_path, GL_VERTEX_SHADER);
	GLuint FragmentShaderID = compile_shader(fragment_file_path, GL_FRAGMENT_SHADER);
	if(!VertexShaderID || !FragmentShaderID)
	{
		ft_putendl("Impossible to open file !");
		return 0;
	}
	check_shader(VertexShaderID);
	check_shader(FragmentShaderID);
	GLuint ProgramID = link_shaders(VertexShaderID, FragmentShaderID);
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	return ProgramID;
}