#include "Explosion.h"

Explosion::Explosion() {
	sprite = NULL;
	posExplosion = glm::vec2(0, 0);
	timer = 40 + rand() % 16;
}

void Explosion::init(ShaderProgram& shaderProgram, Scene *scene) {

	tilesheet.loadFromFile("images/explosion.png", TEXTURE_PIXEL_FORMAT_RGBA);

	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.25, 1.0), &tilesheet, &shaderProgram, scene);
	sprite->setNumberAnimations(1);
	sprite->setAnimationSpeed(0, timer/8);
	sprite->addKeyframe(0, glm::vec2(0,0));
	sprite->addKeyframe(0, glm::vec2(0.25,0));
	sprite->addKeyframe(0, glm::vec2(0.50,0));
	sprite->addKeyframe(0, glm::vec2(0.77,0));
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
void Explosion::renderExplosion(glm::vec2 posPlayer, float angle) {
	if (timer > 0) {
		sprite->render(posPlayer, angle);
		--timer;
	}
}

