#include "Bridge.h"

Bridge::Bridge()
{
	maxdelay = 0;
	delay = 0;
	destroy = false;
	sprite = NULL;
	lower = NULL;
	posBridge = glm::vec2(0, 0);
}

void Bridge::init(ShaderProgram& shaderProgram, int tile, int sizex, int sizey, int new_delay, int pos) {
	program = shaderProgram;
	maxdelay = new_delay;
	delay = new_delay;
	tilesheet.loadFromFile("images/tilesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);

	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.0625f, 0.0625f), &tilesheet, &program);
	sprite->setNumberAnimations(3);
	sprite->setAnimationSpeed(0, 8);
	sprite->addKeyframe(0, glm::vec2(float((tile) % 16) / sizex, float((tile) / 16) / sizey));
	sprite->addKeyframe(0, glm::vec2(float((tile + 2) % 16) / sizex, float((tile) / 16) / sizey));
	sprite->addKeyframe(0, glm::vec2(float((tile + 4) % 16) / sizex, float((tile) / 16) / sizey));
	sprite->addKeyframe(0, glm::vec2(float((tile + 2) % 16) / sizex, float((tile) / 16) / sizey));

	sprite->setAnimationSpeed(2, 1);
	sprite->addKeyframe(2, glm::vec2(float(112 % 16) / sizex, float(112 / 16) / sizey));

	sprite->setAnimationSpeed(1, 1);
	sprite->addKeyframe(1, glm::vec2(float(78 % 16) / sizex, float(78 / 16) / sizey));

	sprite->changeAnimation(0);
	//////////
	lower = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.0625f, 0.0625f), &tilesheet, &program);
	lower->setNumberAnimations(2);
	lower->setAnimationSpeed(0, 1);
	if(pos==0) lower->addKeyframe(0, glm::vec2(float(113 % 16) / sizex, float(113 / 16) / sizey));
	else if (pos==7) lower->addKeyframe(0, glm::vec2(float(115 % 16) / sizex, float(115 / 16) / sizey));
	else lower->addKeyframe(0, glm::vec2(float(114 % 16) / sizex, float(114 / 16) / sizey));

	lower->setAnimationSpeed(1, 1);
	lower->addKeyframe(1, glm::vec2(78.0f / sizex, 78.0f / sizey));
	lower->changeAnimation(0);
}
void Bridge::setDestroy(int type) {
	if (delay == 40) {
		Explosion* e = new Explosion();
		e->init(program);
		e->setPos(glm::vec2(posBridge.x,posBridge.y - type*8));
		exp.push_back(e);

		e = new Explosion();
		e->init(program);
		e->setPos(glm::vec2(posBridge.x, posBridge.y +10));
		exp.push_back(e);
	}
	destroy = true;
	if (delay == 40) {
		sprite->changeAnimation(type); //rumble
		lower->changeAnimation(1);
	}
	--delay;
}
bool Bridge::getDestroy() {
	return destroy;
}
void Bridge::decDelay() {
	--delay;
}

int Bridge::getDelay() {
	return delay;
}
void Bridge::changeAnim(int anim) {
	sprite->changeAnimation(anim);
}

void Bridge::setPos(glm::ivec2 newpos) {
	sprite->setPosition(newpos);
	lower->setPosition(glm::ivec2(newpos.x, newpos.y + 16));
	posBridge = newpos;
}
glm::ivec2 Bridge::getPos() {
	return posBridge;
}
void Bridge::updateBridge(int deltaTime) {
	for (int i = 0; i < exp.size() && destroy; ++i)
		exp[i]->updateExplosion(deltaTime);
	sprite->update(deltaTime);
	lower->update(deltaTime);
}
void Bridge::renderBridge() {

	if (delay >= 0) {
		sprite->render();
		lower->render();
		for (int i = 0; i < exp.size() && destroy; ++i)
			exp[i]->renderExplosion();
	}
}
void Bridge::setMapPos(int pos) {
	mappos = pos;
}

int Bridge::getMapPos() {
	return mappos;
}

