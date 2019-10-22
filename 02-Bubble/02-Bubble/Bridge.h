#pragma once
#include "Sprite.h"
#include "Explosion.h"
class Bridge
{
public:
	Bridge();
	~Bridge();
	void init(ShaderProgram& shaderProgram, int tile, int sizex, int sizey, int delay, int pos);
	void setPos(glm::ivec2 newpos);
	glm::ivec2 getPos();
	void updateBridge(int deltaTime);
	void renderBridge();
	void changeAnim(int anim);
	void setDestroy(int type);
	bool getDestroy();
	int getDelay();
	void decDelay();
	void Bridge::setMapPos(int);
	int Bridge::getMapPos();
private:
	glm::ivec2 posBridge;
	int mappos;
	Sprite* sprite;
	Sprite* lower;
	Texture tilesheet;
	int delay;
	int maxdelay;
	bool destroy;
	ShaderProgram program;
	vector<Explosion *> exp;

};

