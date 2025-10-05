#include "display.h"
#include "renderer.h"

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
