#pragma once
#include "SDL_render.h"
#include "SDL_video.h"

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
};
extern Renderer renderer;

bool InitWindow();
void Setup();
void ProcessInput(bool &isRunning);
void Update();
void Render();
void CleanUp();
