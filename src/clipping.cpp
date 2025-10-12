#include "clipping.h"
#include "model.h"
#include <cmath>

Clipping clipping  = {
	.frustum = {},
};

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Left plane   :  P=(0, 0, 0),     N=(cos(fov/2), 0, sin(fov/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fov/2), 0, sin(fov/2))
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fov/2), sin(fov/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fov/2), sin(fov/2))
///////////////////////////////////////////////////////////////////////////////
Frustum InitFrustumPlanes(float vertFov, float horFov, float zNear, float zFar) {
	float cosHalfVertFov = cos(vertFov / 2);
	float sinHalfVertFov = sin(vertFov / 2);
	float cosHalfHorFov = cos(horFov / 2);
	float sinHalfHorFov = sin(horFov / 2);

	Frustum frustum;

	frustum.planes[NEAR_FRUSTUM_PLANE].point = {0, 0, zNear};
	frustum.planes[NEAR_FRUSTUM_PLANE].normal = {0, 0, 1};

	frustum.planes[FAR_FRUSTUM_PLANE].point = {0, 0, zFar};
	frustum.planes[FAR_FRUSTUM_PLANE].normal = {0, 0, -1};

	frustum.planes[LEFT_FRUSTUM_PLANE].point = {0, 0, 0};
	frustum.planes[LEFT_FRUSTUM_PLANE].normal = {cosHalfHorFov, 0, sinHalfHorFov};

	frustum.planes[RIGHT_FRUSTUM_PLANE].point = {0, 0, 0};
	frustum.planes[RIGHT_FRUSTUM_PLANE].normal = {-cosHalfHorFov, 0, sinHalfHorFov};

	frustum.planes[TOP_FRUSTUM_PLANE].point = {0, 0, 0};
	frustum.planes[TOP_FRUSTUM_PLANE].normal = {0, -cosHalfVertFov, sinHalfVertFov};

	frustum.planes[BOTTOM_FRUSTUM_PLANE].point = {0, 0, 0};
	frustum.planes[BOTTOM_FRUSTUM_PLANE].normal = {0, cosHalfVertFov, sinHalfVertFov};

	return frustum;
}

inline float Vec4GetAxis(const Vec4f &v, Axis axis) {
	switch (axis) {
	case X_AXIS:
		return v.x;
		break;
	case Y_AXIS:
		return v.y;
		break;
	case Z_AXIS:
		return v.z;
		break;
	case W_AXIS:
		return v.w;
		break;
	}
}

float LerpFloat(float a, float b, float t) {
	return a + t * (b - a);
}

void ClipPolygonAxisSide(Axis axis, float side, Polygon &polygon){
	const int startElementCount = polygon.elementCount;

	Vec4f clippedVertices[MAX_NUM_POLY_VERTICES];
	TexCoord clippedUVs[MAX_NUM_POLY_VERTICES];
	int clippedElementCount = 0;

	Vec4f prevVertex = polygon.vertices[startElementCount - 1];
	TexCoord prevTexCoord = polygon.texCoords[startElementCount - 1];
	int prevDot = (side * Vec4GetAxis(prevVertex, axis)) <= Vec4GetAxis(prevVertex, W_AXIS) ? 1 : -1;

	for (int i = 0; i < startElementCount; i++) {
		Vec4f currVertex = polygon.vertices[i];
		TexCoord currTexCoord = polygon.texCoords[i];
		int currDot = (side * Vec4GetAxis(currVertex, axis)) <= Vec4GetAxis(currVertex, W_AXIS) ? 1 : -1;

		//If one of the vertices is inside while the other is outside
		if ((prevDot * currDot) < 0) {
			//Interpolation factor t
			const float t = 
				(Vec4GetAxis(prevVertex, W_AXIS) - Vec4GetAxis(prevVertex, axis) * side) /
				((Vec4GetAxis(prevVertex, W_AXIS) - Vec4GetAxis(prevVertex, axis) * side) -
				(Vec4GetAxis(currVertex, W_AXIS) - Vec4GetAxis(currVertex, axis) * side));

			//Intersection point of the polygon and the plane I = Qp + t(Qc - Qp)
			clippedVertices[clippedElementCount] = prevVertex + ((currVertex - prevVertex) * t);
			clippedUVs[clippedElementCount] = TexCoord {
				.u = LerpFloat(prevTexCoord.u, currTexCoord.u, t),
				.v = LerpFloat(prevTexCoord.v, currTexCoord.v, t),
			};
			clippedElementCount++;
		}

		//Current vertex is inside the plane
		if (currDot > 0) {
			clippedVertices[clippedElementCount] = currVertex;
			clippedUVs[clippedElementCount] = currTexCoord;
			clippedElementCount++;
		}

		prevVertex = currVertex;
		prevTexCoord = currTexCoord;
		prevDot = currDot;
	}

	for (int i = 0; i < clippedElementCount; i++) {
		polygon.vertices[i] = clippedVertices[i];
		polygon.texCoords[i] = clippedUVs[i];
	}
	polygon.elementCount = clippedElementCount;
}

Polygon CreatePolygonFromTriangle(const Triangle &tri) {
	return Polygon{
		.vertices = {tri.points[0], tri.points[1], tri.points[2]},
		.texCoords = {tri.texCoords[0], tri.texCoords[1], tri.texCoords[2]},
		.elementCount = 3,
	};
}

void CreateTrisFromPolygon(Polygon &poly, Triangle tris[], int &count) {
	for (int i = 0; i < poly.elementCount - 2; i++) {
		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;

		tris[i].points[0] = poly.vertices[index0];
		tris[i].points[1] = poly.vertices[index1];
		tris[i].points[2] = poly.vertices[index2];
		tris[i].texCoords[0] = poly.texCoords[index0];
		tris[i].texCoords[1] = poly.texCoords[index1];
		tris[i].texCoords[2] = poly.texCoords[index2];
	}
	count = poly.elementCount - 2;
}
