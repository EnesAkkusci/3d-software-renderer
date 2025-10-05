#include <SDL.h>
#include <iostream>
#include "renderer.h"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_pixels.h"
#include "display.h"
#include "SDL_render.h"
#include "SDL_video.h"

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

	ClearColorBuffer(0xFF000000); //Clear with black
	ClearZBuffer();
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

void Update() {
	//Limiting the FPS
	int timeToWait = renderer.MIN_MS_PER_FRAME - (SDL_GetTicks64() - renderer.msPassedUntilLastFrame);
	if (timeToWait > 0 && timeToWait <= renderer.MIN_MS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	//Time passed between last and this frame. (Converted from ms to seconds)
	renderer.deltaTime = (SDL_GetTicks64() - renderer.msPassedUntilLastFrame) / 1000.0f;
	renderer.msPassedUntilLastFrame = SDL_GetTicks64();
}

void Render() {
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
