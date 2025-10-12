#pragma once
#include "linear_algebra.h"
#include "model.h"
#include <cstdint>

struct Display{
	uint32_t* colorBuffer;
	float* zBuffer;
};
extern Display display;

void ClearColorBuffer(uint32_t color);
void ClearZBuffer();

void DrawGrid(int step);
void DrawPixel(int x, int y, uint32_t color);
void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);
void DrawTriangle(const Triangle &tri, uint32_t color);
void DrawFilledRect(int x, int y, int w, int h, uint32_t color);
void DrawFilledTriangle(const Triangle &tri, uint32_t color);
void DrawTexel(int x, int y, const Triangle &tri, uint32_t *texture, const Vec3f &weights);
void DrawTexturedTriangle(Triangle &tri, uint32_t *texture);

Vec4f GetScreenCoords(const Vec4f &camCoords, const Mat4f &projMat, int windowWidth, int windowHeight, bool project);

void RenderColorBuffer();
