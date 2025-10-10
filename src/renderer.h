#pragma once
#include <vector>
#include <SDL.h>
#include "linear_algebra.h"
#include "model.h"

enum RenderMode {
	TEXTURED,
	FILLED,
	NO_TEXTURE
};

struct Renderer{
	const int MAX_FPS;
	const int MIN_MS_PER_FRAME;
	double deltaTime;
	int msPassedUntilLastFrame;
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	int windowWidth;
	int windowHeight;
	SDL_Texture* sdlColorBufferTexture;
	std::vector<Triangle> trisToRender;
	bool renderWireframe;
	RenderMode renderMode;
	Mat4f projectionMat;
	bool backfaceCulling;
};
extern Renderer renderer;

bool InitWindow();
void Setup();
void ProcessInput(bool &isRunning);
void Update();
void Render();
void CleanUp();
