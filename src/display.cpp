#include "display.h"
#include "linear_algebra.h"
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

void DrawTriangle(const Triangle &tri, Color color) {
	DrawLine(tri.points[0].x, tri.points[0].y, tri.points[1].x, tri.points[1].y, color);
	DrawLine(tri.points[1].x, tri.points[1].y, tri.points[2].x, tri.points[2].y, color);
	DrawLine(tri.points[2].x, tri.points[2].y, tri.points[0].x, tri.points[0].y, color);
}

void DrawFilledRect(int x, int y, int w, int h, Color color) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			DrawPixel(j+x, i+y, color);
		}
	}
}

bool isEdgeTopLeft(const Vec4f &start, const Vec4f &end){
	Vec4f edge = end - start;
	bool isTopEdge = edge.y == 0 and edge.x > 0;
	bool isLeftEdge = edge.y < 0;
	return isTopEdge or isLeftEdge;
}

//Bounding Box Barycentric Rasterization
void DrawFilledTriangle(const Triangle &tri, Color color) {
	Vec4f v0 = tri.points[0];
	Vec4f v1 = tri.points[1];
	Vec4f v2 = tri.points[2];
	
	//Finding the bounds of the box
	int xMin = floor(std::min({v0.x, v1.x, v2.x}));
	int yMin = floor(std::min({v0.y, v1.y, v2.y}));
	int xMax = ceil(std::max({v0.x, v1.x, v2.x}));
	int yMax = ceil(std::max({v0.y, v1.y, v2.y}));

	//The constant deltas of the area that we get by 2D crossing the two edges of the smaller triangle for the barycentric weights.
	//Giving us the numerator.
	float deltaW0Col = (v1.y - v2.y);
	float deltaW1Col = (v2.y - v0.y);
	float deltaW2Col = (v0.y - v1.y);
	float deltaW0Row = (v2.x - v1.x);
	float deltaW1Row = (v0.x - v2.x);
	float deltaW2Row = (v1.x - v0.x);

	//Area of the bigger triangle for the denominator of the barycentric weights.
	// float area = Vec2Cross(Vec2f(v1 - v0), Vec2f(v2 - v0));

	//Fill convention (top-left rasterization rule)
	//if the edge is not top left we will forego fill rights
	float bias0 = isEdgeTopLeft(v1, v2) ? 0 : -0.0001;
	float bias1 = isEdgeTopLeft(v2, v0) ? 0 : -0.0001;
	float bias2 = isEdgeTopLeft(v0, v1) ? 0 : -0.0001;

	//The starting pixel
	Vec4f p0 = {xMin + 0.5f, yMin + 0.5f, 0, 0};
	//The starting values for the numerator of the barycentric calculation for this row
	float w0Row = Vec2Cross(Vec2f(v2 - v1), Vec2f(p0 - v1)) + bias0;
	float w1Row = Vec2Cross(Vec2f(v0 - v2), Vec2f(p0 - v2)) + bias1;
	float w2Row = Vec2Cross(Vec2f(v1 - v0), Vec2f(p0 - v0)) + bias2;

	//Loop over the candidate pixels within the boundry
	for (int y = yMin; y < yMax; y++) {
		float w0 = w0Row;
		float w1 = w1Row;
		float w2 = w2Row;

		for (int x = xMin; x < xMax; x++) {
			bool isInside = w0 >= 0 and w1 >= 0 and w2 >= 0;
			if (isInside) { DrawPixel(x, y, color); }

			w0 += deltaW0Col;
			w1 += deltaW1Col;
			w2 += deltaW2Col;
		}

		w0Row += deltaW0Row;
		w1Row += deltaW1Row;
		w2Row += deltaW2Row;
	}
}

Vec4f GetScreenCoords(const Vec4f &camCoords, const Mat4f &projMat, int windowWidth, int windowHeight) {
	//Camera space -> NDC
	Vec4f v = Vec4MultMat4(camCoords, projMat);
	//Perspective divide
	if(v.w != 0.0) {
		v.x /= v.w;
		v.y /= v.w;
		v.z /= v.w;
	};
	//NDC -> Raster space
	v.y *= -1;
	v.x *= windowWidth/2.0;
	v.y *= windowHeight/2.0;
	v.x += windowWidth/2.0;
	v.y += windowHeight/2.0;

	return v;
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
