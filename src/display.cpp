#include "display.h"
#include "renderer.h"
#include <iostream>
#include <ostream>

Display display = {
	.colorBuffer = nullptr,
	.zBuffer = nullptr,
};

void ClearColorBuffer(Color color) {
	for (int i = 0; i < renderer.windowWidth * renderer.windowHeight; i++)
		display.colorBuffer[i] = color;
}

void ClearZBuffer() {
	for (int i = 0; i < renderer.windowWidth * renderer.windowHeight; i++)
		display.zBuffer[i] = 0.0;
}

void RenderColorBuffer() {
	SDL_UpdateTexture(
		renderer.sdlColorBufferTexture, 
		NULL, 
		display.colorBuffer, 
		(int)(sizeof(uint32_t) * renderer.windowWidth)
	);

	SDL_RenderCopy(renderer.sdlRenderer, renderer.sdlColorBufferTexture, NULL, NULL);
}

void DrawGrid(int step) {
	for(int i = 0; i < renderer.windowHeight; i += step) {
		for(int j = 0; j < renderer.windowWidth; j += step) {
				display.colorBuffer[(i * renderer.windowWidth) + j] = 0xFF606060;
		}
	}
}

void DrawPixel(int x, int y, Color color) {
	if(x >= 0 and y >= 0 and x < renderer.windowWidth and y < renderer.windowHeight)
		display.colorBuffer[(y * renderer.windowWidth) + x] = color;
	else std::cout << "Out of window bounds DrawPixel() call." << std::endl;
}

//Bresensham's line algorithm
void DrawLine(int x0, int y0, int x1, int y1, Color color) {
	int dx = abs(x1 - x0);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0);
	int sy = y0 < y1 ? 1 : -1;
	int error = dx + dy;

	while (true) {
		DrawPixel(x0, y0, color);
		if(x0 == x1 and y0 == y1) break;
		int e2 = 2 * error;
		if (e2 >= dy) { //advance in x
			error = error + dy;
			x0 = x0 + sx;
		}
		if (e2 <= dx) { //advance in y
			error = error + dx;
			y0 = y0 + sy;
		}
	}
}

void DrawTriangle(Triangle tri, Color color) {
	DrawLine(tri.points[0].x, tri.points[0].y, tri.points[1].x, tri.points[1].y, color);
	DrawLine(tri.points[1].x, tri.points[1].y, tri.points[2].x, tri.points[2].y, color);
	DrawLine(tri.points[2].x, tri.points[2].y, tri.points[0].x, tri.points[0].y, color);
}

void DrawRect(int x, int y, int w, int h, Color color) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			DrawPixel(j+x, i+y, color);
		}
	}
}
