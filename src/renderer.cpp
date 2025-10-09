#include <SDL.h>
#include <immintrin.h>
#include <iostream>
#include "renderer.h"
#include "display.h"
#include "linear_algebra.h"
#include "model.h"
#include "camera.h"

static const int FPS = 144;
Renderer renderer = {
	.MAX_FPS = FPS,
	.MIN_MS_PER_FRAME = 1000/FPS,
	.deltaTime = 0,
	.msPassedUntilLastFrame = 0,
	.sdlWindow = nullptr,
	.sdlRenderer = nullptr,
	.windowWidth = 1920,
	.windowHeight = 1080,
	.sdlColorBufferTexture = nullptr,
	.trisToRender = {},
	.renderWireframe = false,
	.renderMode = RenderMode::FILLED,
	.projectionMat = {},
};

bool InitWindow() {
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cout << "Error initializing SDL." << std::endl;
		return false;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	int fullScreenWidth = displayMode.w;
	int fullScreenHeight = displayMode.h;

	renderer.sdlWindow = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		fullScreenWidth,
		fullScreenHeight,
		SDL_WINDOW_BORDERLESS
	);
	if (!renderer.sdlWindow) {
		std::cout << "Error initalizing the SDL window." << std::endl;
		return false;
	}

	renderer.sdlRenderer = SDL_CreateRenderer(renderer.sdlWindow, -1, 0);
	if(!renderer.sdlRenderer) {
		std::cout << "Error creating the SDL renderer." << std::endl;
		return false;
	}

	return true;
}

void Setup() {
	display.colorBuffer = new Color[renderer.windowWidth * renderer.windowHeight];
	renderer.sdlColorBufferTexture = SDL_CreateTexture(
		renderer.sdlRenderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		renderer.windowWidth,
		renderer.windowHeight
	);
	display.zBuffer = new float[renderer.windowWidth * renderer.windowHeight];

	float verticalFov = M_PI/3.0;
	float horizontalFov = atan(tan(verticalFov / 2) * ((float)renderer.windowWidth / renderer.windowHeight)) * 2.0;
	float zNear = 0.1; float zFar = 100;
	renderer.projectionMat = GetPerspectiveMat(
		verticalFov, 
		renderer.windowWidth, 
		renderer.windowHeight, 
		zNear, zFar
	);

	ClearColorBuffer(0xFF000000); //Clear with black
	ClearZBuffer();

	LoadObjFile(model.mesh, "cube.obj");
}

void ProcessInput(bool &isRunning){
	SDL_Event sdlEvent;
	while(SDL_PollEvent(&sdlEvent)) {
		switch (sdlEvent.type){
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.sym) {
			case SDLK_ESCAPE:
				isRunning = false;
				break;
			}
		}
	}
}

float rotation  = 0.0;
void Update() {
	//Limiting the FPS
	int timeToWait = renderer.MIN_MS_PER_FRAME - (SDL_GetTicks64() - renderer.msPassedUntilLastFrame);
	if (timeToWait > 0 && timeToWait <= renderer.MIN_MS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	//Setting up the worldToCameraMatrix
	Mat4f yawMat = GetRotationMat(0, camera.yawAngle, 0);
	camera.direction = Vec4MultMat4({0,0,1,0}, yawMat);
	Mat4f worldToCameraMatrix = GetLookTowardsMat(
		camera.position, 
		camera.direction, 
		{0,1,0}
	);

	//NOTE: Temporary
	rotation += 0.01;

	//Setting up the tranformation matrices
	Mat4f scaleMat = GetScaleMat(1, 1, 1);
	Mat4f rotMat = GetRotationMat(rotation,rotation,rotation);
	Mat4f translationMat = GetTranslationMat(0, 0, 5);

	//Transformation and projection of the model vertices
	for (Face &face : model.mesh.faces){
		Vec3f faceVertices[3] = {face.a,face.b,face.c};

		Vec3f transformedVertices[3];
		Triangle triToRender;
		for (int i = 0; i < 3; i++) {
			Vec3f v = faceVertices[i];
			//Scale -> Rotate -> Translate
			v = Vec4MultMat4(Vec4f(v), scaleMat);
			v = Vec4MultMat4(Vec4f(v), rotMat);
			v = Vec4MultMat4(Vec4f(v), translationMat);

			//World space -> Camera space
			v = Vec4MultMat4(Vec4f(v), worldToCameraMatrix);

			//Camera space -> Raster space
			Vec4f projectedVertex = GetScreenCoords(
				v, 
				renderer.projectionMat, 
				renderer.windowWidth, 
				renderer.windowHeight
			);

			// transformedVertices[i] = projectedVertex;
			triToRender.points[i] = projectedVertex;
		}
		renderer.trisToRender.push_back(triToRender);
	}

	//Time passed between last and this frame. (Converted from ms to seconds)
	renderer.deltaTime = (SDL_GetTicks64() - renderer.msPassedUntilLastFrame) / 1000.0f;
	renderer.msPassedUntilLastFrame = SDL_GetTicks64();
}

void Render() {
	DrawGrid(10);

	switch (renderer.renderMode) {
	case RenderMode::NO_TEXTURE :
		break;
	case RenderMode::FILLED :
		for (const Triangle &tri : renderer.trisToRender) { 
			DrawFilledTriangle(tri, 0xFFFFFFFF);
		}
		break;
	case RenderMode::TEXTURED :
		break;
	}

	if (renderer.renderWireframe) {
		for (const Triangle &tri : renderer.trisToRender) { 
			DrawTriangle(tri, 0xFF00FF00);
		}
	}

	renderer.trisToRender.clear();

	RenderColorBuffer();
	ClearColorBuffer(0xFF000000); //Clear with black
	ClearZBuffer();
	SDL_RenderPresent(renderer.sdlRenderer);
}

void CleanUp() {
	delete[] display.colorBuffer;
	delete[] display.zBuffer;
	SDL_DestroyTexture(renderer.sdlColorBufferTexture);
	SDL_DestroyRenderer(renderer.sdlRenderer);
	SDL_DestroyWindow(renderer.sdlWindow);
	SDL_Quit();
}
