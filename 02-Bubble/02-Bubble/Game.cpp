#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"


void Game::init()
{
	bPlay = true;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	scene.init(0);

}

bool Game::update(int deltaTime)
{
	if (getKey('0')) {
		scene.deleteEntities();
		scene.init(0);
	}

	if (getKey('1')) {
		scene.deleteEntities();
		scene.init(1);
	}

	if (getKey('2')) {
		scene.deleteEntities();
		scene.init(2);
	}
	if (getKey('3')) {
		scene.deleteEntities();
		scene.init(3);
	}
	if (getKey('4')) {
		scene.deleteEntities();
		scene.init(4);
	}
	scene.update(deltaTime);
	
	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

void Game::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}





