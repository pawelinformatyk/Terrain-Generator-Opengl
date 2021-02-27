#pragma once

struct CameraMouse
{
	int mouse_positionX, mouse_positionY;
	int button;
	double cameraX, cameraZ, cameraD, previous_cameraX, previous_cameraZ, previous_cameraD;
};