#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"

#pragma once
class Medalla
{
public:
	Medalla();
	~Medalla();

	void iniMedalla(glm::ivec2 offset, ShaderProgram& shaderProgram, string textu);
	void Medalla::setPos(glm::vec2 newpos);
	void render();

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	Texture tilesheet;
	glm::vec2 position;
	ShaderProgram *program;

};

