#include "Explosion.h"

Explosion::Explosion() {
	sprite = NULL;
	posExplosion = glm::vec2(0, 0);
}

void Explosion::init(ShaderProgram& shaderProgram) {

	tilesheet.loadFromFile("images/explosion.png", TEXTURE_PIXEL_FORMAT_RGBA);

	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.25, 1.0), &tilesheet, &shaderProgram);
	sprite->setNumberAnimations(1);
	sprite->setAnimationSpeed(0, 5);
	sprite->addKeyframe(0, glm::vec2(0,0));
	sprite->addKeyframe(0, glm::vec2(0.25,0));
	sprite->addKeyframe(0, glm::vec2(0.50,0));
	sprite->addKeyframe(0, glm::vec2(0.75,0));
	sprite->changeAnimation(0);

}
void Explosion::setPos(glm::ivec2 newpos) {
	sprite->setPosition(newpos);
	posExplosion = newpos;
}
glm::ivec2 Explosion::getPos() {
	return posExplosion;
}
void Explosion::updateExplosion(int deltaTime) {
	sprite->update(deltaTime);
}
void Explosion::renderExplosion() {
	sprite->render();
}

