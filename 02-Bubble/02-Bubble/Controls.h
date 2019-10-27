#pragma once
#include "Sprite.h"
class Controls
{
public:
	Controls();
	~Controls();
	void ini(ShaderProgram& ShaderProgram, Scene * scene, glm::vec2 offset);
	void update(int deltaTime);
	void render(glm::vec2 posPlayer, float angle);

private:

	Texture tex;
	ShaderProgram *program;
	Sprite *background;

};

