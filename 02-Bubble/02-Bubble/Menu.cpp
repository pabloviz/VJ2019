#include "Menu.h"



Menu::Menu()
{
	background = NULL;
	player1 = NULL;
	player2 = NULL;
	select = NULL;
}


Menu::~Menu()
{

}

void Menu::ini(ShaderProgram& shaderProgram, Scene * scene, glm::vec2 offset) {
	selected = 1;
	pos = offset;
	posini = offset.x + 128;
	tex.loadFromFile("images/menu1.png", TEXTURE_PIXEL_FORMAT_RGBA);

	background = Sprite::createSprite(glm::ivec2(224, 224), glm::vec2(1.0, 0.94), &tex, &shaderProgram, scene);
	background->setNumberAnimations(1);
	background->setAnimationSpeed(0, 1);
	background->addKeyframe(0, glm::vec2(0, 0));
	background->changeAnimation(0);
	background->setPosition(offset + glm::vec2(128,0));

	player1 = Sprite::createSprite(glm::ivec2(64, 16), glm::vec2(0.294, 0.0588), &tex, &shaderProgram, scene);
	player1->setNumberAnimations(2);
	player1->setAnimationSpeed(0, 1);
	player1->addKeyframe(0, glm::vec2(0.0625, 0.941));
	player1->setAnimationSpeed(1, 2);
	player1->addKeyframe(1, glm::vec2(0, 0));
	player1->addKeyframe(1, glm::vec2(0.0625, 0.941));
	player1->changeAnimation(1);
	player1->setPosition(offset + glm::vec2(15*3, 16*9));

	player2 = Sprite::createSprite(glm::ivec2(64, 16), glm::vec2(0.294, 0.0588), &tex, &shaderProgram, scene);
	player2->setNumberAnimations(2);
	player2->setAnimationSpeed(0, 1);
	player2->addKeyframe(0, glm::vec2(0.375, 0.941));
	player2->setAnimationSpeed(1, 2);
	player2->addKeyframe(1, glm::vec2(0, 0));
	player2->addKeyframe(1, glm::vec2(0.375, 0.941));
	player2->changeAnimation(0);
	player2->setPosition(offset + glm::vec2(15*3, 16 * 10));

	select = Sprite::createSprite(glm::ivec2(14, 14), glm::vec2(0.0625, 0.0588), &tex, &shaderProgram, scene);
	select->setNumberAnimations(1);
	select->setAnimationSpeed(0, 1);
	select->addKeyframe(0, glm::vec2(0, 0.941));
	select->changeAnimation(0);
	select->setPosition(offset + glm::vec2(16 * 2, 16 * 9));


}

void Menu::changeselected(int newselect) {
	selected = newselect;
	if (selected == 1) {
		player1->changeAnimation(1);
		player2->changeAnimation(0);
		select->setPosition(pos + glm::vec2(16 * 2, 16 * 9));
	}
	else if (selected == 2) {
		player1->changeAnimation(0);
		player2->changeAnimation(1);
		select->setPosition(pos + glm::vec2(16 * 2, 16 * 10));
	}

}

int Menu::getselected() {
	if (posini > 0) return -1;
	return selected;
}

void Menu::setposini(int newini) {
	posini = newini;
}

void Menu::update(int deltaTime) {

	if (posini > 0) {
		--posini;
		background->setPosition(glm::vec2(pos.x + posini, pos.y));
	}

	background->update(deltaTime);
	player1->update(deltaTime);
	player2->update(deltaTime);
	select->update(deltaTime);
 
}

void Menu::render(glm::vec2 posPlayer, float angle) {
	background->render(posPlayer,angle);
	if (posini <= 0) {
		player1->render(posPlayer, angle);
		player2->render(posPlayer, angle);
		select->render(posPlayer, angle);
	}

}