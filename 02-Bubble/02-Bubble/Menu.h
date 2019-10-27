#pragma once
#include "Sprite.h"
class Menu
{
public:
	Menu();
	~Menu();
	void ini(ShaderProgram& ShaderProgram, Scene * scene, glm::vec2 offset);
	void update(int deltaTime);
	void render(glm::vec2 posPlayer, float angle);
	void changeselected(int newselect);
	int getselected(); 
	void setposini(int newini);
private:

	Texture tex;
	ShaderProgram *program;
	Sprite *background, *player1, *player2, *select;
	int selected;
	glm::vec2 pos;
	int posini;


};

