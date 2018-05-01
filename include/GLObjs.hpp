/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GLObjs.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abartz <abartz@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/21 16:47:42 by abartz            #+#    #+#             */
/*   Updated: 2018/05/01 14:24:27 by abartz           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include "SFML/Graphics/Shader.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	unsigned int	id;
	std::string		type;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> ids, std::vector<Texture> texts);
	void Draw(sf::Shader shader);

private:
	unsigned int _vao, _vbo, _ebo;

	void setupMesh();
};
