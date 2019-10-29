#include <glm/glm.hpp>
#include "Player.h"

class Camera {

public:

	void update(int deltaTime, glm::ivec2 posplayer);
	glm::mat4 calcProj();
	void setCameraPos(glm::ivec2 newpos);
	glm::ivec2 getCameraPos();
	void setTV(bool TV);
	void setScroll(bool scroll);

private:
	void rotateCamera(float angle);

	glm::ivec2 posCamera;
	float angle = 0.f;
	bool TV;
	bool scroll;
	//float CameraXspeed, CameraYSpeed;

};

