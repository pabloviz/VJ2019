#include "Medalla.h"	
#include <glm\gtc\matrix_transform.hpp>

void Medalla::iniMedalla(glm::ivec2 offset, ShaderProgram& Sprogram, string textu) {

	program = &Sprogram;
	//position = posTile; //used for transform
	glm::ivec2 posTile = offset;
	bool b = tilesheet.loadFromFile(textu, TEXTURE_PIXEL_FORMAT_RGBA);
	//bool b = tilesheet.loadFromFile("images/tilesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);


	glm::vec2 texCoordTile[2], halfTexel, tileTexSize;
	vector<float> vertices;
	tileTexSize = glm::vec2(1.f / 1, 1.f / 1); // divided per tilemap size in tiles
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());

	texCoordTile[0] = glm::vec2(0.0f, 0.0f);
	//texCoordTile[1] = glm::vec2(16, 16);
	texCoordTile[1] = texCoordTile[0] + tileTexSize;
	//texCoordTile[0] += halfTexel;
	texCoordTile[1] -= halfTexel;
	int size = 16;
	// First triangle
	vertices.push_back(posTile.x); vertices.push_back(posTile.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(posTile.x + size); vertices.push_back(posTile.y);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(posTile.x + size); vertices.push_back(posTile.y + size);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
	// Second triangle
	vertices.push_back(posTile.x); vertices.push_back(posTile.y);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
	vertices.push_back(posTile.x + size); vertices.push_back(posTile.y + size);
	vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
	vertices.push_back(posTile.x); vertices.push_back(posTile.y + size);
	vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * 1 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program->bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program->bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}

void Medalla::setPos(glm::vec2 newpos) {
	position = newpos;
}



void Medalla::render() {
	
	glm::mat4 modelview = glm::mat4(1.0f);
	modelview = glm::translate(modelview, glm::vec3(position.x, position.y, 0.f));
	program->setUniformMatrix4f("modelview", modelview);
	program->setUniform2f("texCoordDispl", 0.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 1);
	glDisable(GL_TEXTURE_2D);
}


Medalla::Medalla()
{
}


Medalla::~Medalla()
{
}
