#pragma once
#include "linear_algebra.h"

struct Camera {
	Vec3f position;
	Vec3f direction;
	float speed;
	float yawAngle;
};

extern Camera camera;
