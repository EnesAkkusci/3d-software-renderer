#pragma once
#include "linear_algebra.h"
#include <vector>

//UV coords of point in a texture
struct TexCoord {
	float u,v;
};

//OBJ file face (holding indices)
struct Face {
	Vec3f a,b,c; //Model coords for the point of the triangle
	TexCoord aUV, bUV, cUV; //UV indices for the point of that face
};

//Raster space triangle
struct Triangle {
	Vec4f points[3];
	TexCoord texCoords[3];
};

struct Mesh {
	std::vector<Face> faces;
};

struct Model {
	Mesh mesh;
	int textureWidth;
	int textureHeight;
	uint32_t *meshTexture;
};
extern Model model;

void LoadObjFile(Mesh &mesh, const char* filename);
void UnloadObjFile(Mesh &mesh);

void LoadPngTexture(Model &model, const char* filename);
void UnloadPngTexture(Model &model);
