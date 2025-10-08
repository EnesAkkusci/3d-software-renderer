#pragma once
#include "linear_algebra.h"
#include "model.h"
#include <cstdint>

using Color = uint32_t;

struct Display{
	Color* colorBuffer;
	float* zBuffer;
};
extern Display display;

void ClearColorBuffer(Color color);
void ClearZBuffer();

void DrawGrid(int step);
void DrawPixel(int x, int y, Color color);
void DrawLine(int x0, int y0, int x1, int y1, Color color);
void DrawTriangle(const Triangle &tri, Color color);
void DrawRect(int x, int y, int w, int h, Color color);

Vec4f GetScreenCoords(const Vec4f &camCoords, const Mat4f &projMat, int windowWidth, int windowHeight);

void RenderColorBuffer();
