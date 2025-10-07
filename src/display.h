#pragma once
#include <cstdint>

using Color = uint32_t;

struct Display{
	Color* colorBuffer;
	float* zBuffer;
};
extern Display display;

void ClearColorBuffer(Color color);
void ClearZBuffer();

void RenderColorBuffer();

void DrawGrid(int step);
