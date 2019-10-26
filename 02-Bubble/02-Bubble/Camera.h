#include <glm/glm.hpp>
#include "Player.h"

class Camera {

public:

	void update(int deltaTime, glm::ivec2 posplayer);
	glm::mat4 calcProj();
	void setCameraPos(glm::ivec2 newpos);
	glm::ivec2 getCameraPos();

private:
	void rotateCamera(float angle);

	glm::ivec2 posCamera;
	float angle = 0.f;
	bool TV = true;
	//float CameraXspeed, CameraYSpeed;

};

