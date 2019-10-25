#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "Player.h" //molaria posar aqui el size saps
//#include "Game.h"

#define CAMERA_WIDTH 14*16
#define CAMERA_HEIGHT 14*16

#define TMP_PLAYER_WIDTH 50
#define TMP_PLAYER_HEIGHT 50

void Camera::update(int deltaTime, glm::ivec2 posplayer) {
	posplayer.x += TMP_PLAYER_WIDTH;
	int diff = posplayer.x - posCamera.x;
	if (diff > CAMERA_WIDTH/2 && (216*16-posCamera.x)>=CAMERA_WIDTH) {
		posCamera.x = posplayer.x - CAMERA_WIDTH/2;
	}
}

glm::mat4 Camera::calcProj() {
	return glm::ortho((float)posCamera.x, (float)(posCamera.x + CAMERA_WIDTH), (float)(posCamera.y+ CAMERA_WIDTH), (float)posCamera.y);
}

void Camera::setCameraPos(glm::ivec2 newpos) {
	posCamera = newpos;
	//posCamera.x = (newpos.x+ TMP_PLAYER_WIDTH) - CAMERA_WIDTH/2;
	//posCamera.y = (newpos.y+ TMP_PLAYER_HEIGHT) - CAMERA_HEIGHT / 2 - 1;
}

glm::ivec2 Camera::getCameraPos() {
	return posCamera;
}