#include "Controls.h"



Controls::Controls()
{
	background = NULL;

}


Controls::~Controls()
{

}

void Controls::ini(ShaderProgram& shaderProgram, Scene * scene, glm::vec2 offset, string image) {
	tex.loadFromFile(image, TEXTURE_PIXEL_FORMAT_RGBA);

	background = Sprite::createSprite(glm::ivec2(224, 224), glm::vec2(1.0, 1.0), &tex, &shaderProgram, scene);
	background->setNumberAnimations(1);
	background->setAnimationSpeed(0, 1);
	background->addKeyframe(0, glm::vec2(0, 0));
	background->changeAnimation(0);
	background->setPosition(offset);

}

void Controls::update(int deltaTime) {
	background->update(deltaTime);

}

void Controls::render(glm::vec2 posPlayer, float angle) {
	background->render(posPlayer, angle);

}
