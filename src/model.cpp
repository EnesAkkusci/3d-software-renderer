#include <cstdint>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include "model.h"
#include "linear_algebra.h"

Model model;

void LoadObjFile(Mesh &mesh, const char* filename){
	std::string fullPath = std::string(ASSETS_PATH) + filename;
	std::ifstream file(fullPath);
	if(file.fail()) {
		std::cout << "Couldn't open obj file: " << filename << std::endl;
		return;
	}

	std::vector<Vec3f> vertices;
	std::vector<TexCoord> texCoords;

	char lineBuff[1024];
	while (file.getline(lineBuff, sizeof(lineBuff))){
		//Vertex position line
		if (strncmp(lineBuff, "v ", 2) == 0) {
			Vec3f v;
			sscanf_s(lineBuff, "v %f %f %f", &v.x, &v.y, &v.z);
			vertices.push_back(v);
		}

		//Vertex texture line
		if (strncmp(lineBuff, "vt ", 3) == 0) {
			TexCoord t;
			sscanf_s(lineBuff, "vt %f %f", &t.u, &t.v);
			texCoords.push_back(t);
		}

		//Face line
		if (strncmp(lineBuff, "f ", 2) == 0) {
			int vertexIndices[3];
			int textureIndices[3];
			int _; //Normals are unused (for now(?))

			sscanf_s(
                lineBuff, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&vertexIndices[0], &textureIndices[0], &_,
				&vertexIndices[1], &textureIndices[1], &_,
				&vertexIndices[2], &textureIndices[2], &_
			);

			Face f = {
				.a = vertices[vertexIndices[0] - 1],
				.b = vertices[vertexIndices[1] - 1],
				.c = vertices[vertexIndices[2] - 1],
				.aUV = texCoords[textureIndices[0] - 1],
				.bUV = texCoords[textureIndices[1] - 1],
				.cUV = texCoords[textureIndices[2] - 1]
			};
			mesh.faces.push_back(f);
		}
	}
}

void UnloadObjFile(Mesh &mesh) {
	mesh.faces.clear();
}

void LoadPngTexture(Model &model, const char* filename) {
	stbi_set_flip_vertically_on_load(1);

	std::string fullPath = std::string(ASSETS_PATH) + filename;
	int w,h,n;
	unsigned char* data = stbi_load(fullPath.c_str(), &w, &h, &n, STBI_rgb_alpha);
	if (!data) {
		std::cout << "Couldn't load png file: " << filename << ", " << stbi_failure_reason() << std::endl;
	}

	model.textureWidth = w;
	model.textureHeight = h;
	model.meshTexture = (uint32_t*)data;
}

void UnloadPngTexture(Model &model) {
	if (model.meshTexture) {
		stbi_image_free((void*) model.meshTexture);
		model.meshTexture = nullptr;
		model.textureWidth = 0;
		model.textureHeight = 0;
	}
}
