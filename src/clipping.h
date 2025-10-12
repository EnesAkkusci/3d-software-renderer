#pragma once
#include "linear_algebra.h"
#include "model.h"

const int MAX_NUM_POLY_VERTICES = 16;
const int MAX_NUM_POLY_TRIS = 16;

enum FrustumPlane {
	NEAR_FRUSTUM_PLANE,
	FAR_FRUSTUM_PLANE,
	LEFT_FRUSTUM_PLANE,
	RIGHT_FRUSTUM_PLANE,
	TOP_FRUSTUM_PLANE,
	BOTTOM_FRUSTUM_PLANE,

	FRUSTUM_PLANE_COUNT
};

enum Axis {
	X_AXIS,
	Y_AXIS,
	Z_AXIS,
	W_AXIS
};

struct Plane{
	Vec3f point;
	Vec3f normal;
};

struct Frustum {
	Plane planes[FRUSTUM_PLANE_COUNT];
};

struct Polygon {
	Vec4f vertices[MAX_NUM_POLY_VERTICES];
	TexCoord texCoords[MAX_NUM_POLY_VERTICES];
	int elementCount;
};

struct Clipping {
	Frustum frustum;
};
extern Clipping clipping;

Frustum InitFrustumPlanes(float vertFov, float horFov, float zNear, float zFar);
void ClipPolygonAxisSide(Axis axis, float side, Polygon &polygon);
Polygon CreatePolygonFromTriangle(const Triangle &tri);
void CreateTrisFromPolygon(Polygon &poly, Triangle tris[], int &count);
