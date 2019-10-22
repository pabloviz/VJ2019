#pragma once
#include "Sprite.h"
class Explosion
{
public:
	Explosion();
	~Explosion();
	void init(ShaderProgram& shaderProgram);

	void setPos(glm::ivec2 newpos);
	glm::ivec2 getPos();
	void updateExplosion(int deltaTime);
	void renderExplosion();

private:
	glm::ivec2 posExplosion;
	Sprite* sprite;
	Texture tilesheet;

};