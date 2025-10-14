#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <SDL.h>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <cmath>
#include <numbers>
#include "renderer.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "display.h"
#include "linear_algebra.h"
#include "model.h"
#include "camera.h"
#include "clipping.h"

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
	.renderMode = RenderMode::TEXTURED,
	.projectionMat = {},
	.backfaceCulling = true,
	.rotation = {},
	.showcase = false,
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
	display.colorBuffer = new uint32_t[renderer.windowWidth * renderer.windowHeight];
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
	clipping.frustum = InitFrustumPlanes(verticalFov, horizontalFov, zNear, zFar);
	renderer.projectionMat = GetPerspectiveMat(
		verticalFov, 
		renderer.windowWidth, 
		renderer.windowHeight, 
		zNear, zFar
	);

	ClearColorBuffer(0xFF000000); //Clear with black
	ClearZBuffer();

	LoadObjFile(model.mesh, "crab.obj");
	LoadPngTexture(model, "crab.png");

	//Setting up ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// ImGuiIO& io = ImGui::GetIO(); (void)io;
	// io.FontGlobalScale = 2.0f;
	// ImGui::GetStyle().ScaleAllSizes(2.0);
	ImGui_ImplSDL2_InitForSDLRenderer(renderer.sdlWindow, renderer.sdlRenderer);
	ImGui_ImplSDLRenderer2_Init(renderer.sdlRenderer);
}

void ProcessInput(bool &isRunning){
	SDL_Event sdlEvent;
	while(SDL_PollEvent(&sdlEvent)) {
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		switch (sdlEvent.type){
		case SDL_QUIT: isRunning = false; break;
		case SDL_KEYDOWN:
			switch (sdlEvent.key.keysym.sym) {
			case SDLK_ESCAPE: isRunning = false; break;
			case SDLK_w: //Move forward
				camera.position = camera.position + 
					camera.direction * (camera.speed * renderer.deltaTime);
				break;
			case SDLK_a: //Move left
				camera.position = camera.position + 
					Vec3Cross({0,1,0}, camera.direction) * (-camera.speed * renderer.deltaTime);
				break;
			case SDLK_s: //Move backward
				camera.position = camera.position + 
					camera.direction * (-camera.speed * renderer.deltaTime);
				break;
			case SDLK_d: //Move right
				camera.position = camera.position + 
					Vec3Cross({0,1,0}, camera.direction) * (camera.speed * renderer.deltaTime);
				break;
			}
		}
	}
}

void RunImGui(SDL_Renderer *renderer, Vec3f &rotation, bool &showcase, RenderMode &renderMode, bool &wireframe, bool &backface) {
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	// ImGui::ShowDemoWindow();
	
	if(ImGui::Begin("Model Controls")){

		ImGui::NewLine();
		const char* renderModeLabels[] = {"Textured", "Filled", "No Texture"};
		int mode = (int)renderMode;
		if(ImGui::Combo("Render mode", &mode, renderModeLabels, IM_ARRAYSIZE(renderModeLabels))) {
			renderMode = (RenderMode)mode;
		}
		ImGui::Checkbox("Wireframe", &wireframe);
		ImGui::SameLine();
		ImGui::Checkbox("Backface culling", &backface);
		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		ImGui::Checkbox("Showcase", &showcase);
		ImGui::SameLine();
		ImGui::BeginDisabled(showcase);
		if(ImGui::Button("Reset rotation")) {
			rotation = {0,0,0};
		}
		ImGui::SliderAngle("X (pitch)", &rotation.x, 0.0f, 360.0f);
		ImGui::SliderAngle("Y (yaw)",   &rotation.y, 0.0f, 360.0f);
		ImGui::SliderAngle("Z (roll)",  &rotation.z, 0.0f, 360.0f);
		ImGui::EndDisabled();
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

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

	if (renderer.showcase) {
		// renderer.rotation = (renderer.rotation + (1 * renderer.deltaTime));
		constexpr float TAU = 2.0f * std::numbers::pi_v<float>;
		renderer.rotation.x = std::fmod(renderer.rotation.x + 1.0f * renderer.deltaTime, TAU);
		renderer.rotation.y = std::fmod(renderer.rotation.y + 1.0f * renderer.deltaTime, TAU);
		renderer.rotation.z = std::fmod(renderer.rotation.x + 1.0f * renderer.deltaTime, TAU);
	}

	//Setting up the tranformation matrices
	Mat4f scaleMat = GetScaleMat(1, 1, 1);
	Mat4f rotMat = GetRotationMat(renderer.rotation.x,renderer.rotation.y,renderer.rotation.z);
	Mat4f translationMat = GetTranslationMat(0, 0, 5);

	//Transformation and projection of the model vertices
	for (Face &face : model.mesh.faces){
		Vec3f faceVertices[3] = {face.a,face.b,face.c};
		Vec3f faceNormal = {};
		Vec3f cameraSpaceVertices[3];

		for (int i = 0; i < 3; i++) {
			Vec3f v = faceVertices[i];
			//Scale -> Rotate -> Translate
			v = Vec4MultMat4(Vec4f(v), scaleMat);
			v = Vec4MultMat4(Vec4f(v), rotMat);
			v = Vec4MultMat4(Vec4f(v), translationMat);

			//World space -> Camera space
			v = Vec4MultMat4(Vec4f(v), worldToCameraMatrix);

			cameraSpaceVertices[i] = v;

			faceNormal = Vec3Cross(
				{cameraSpaceVertices[1] - cameraSpaceVertices[0]},
				{cameraSpaceVertices[2] - cameraSpaceVertices[0]}
			);
		}

		if(renderer.backfaceCulling) {
			//The reason for using 0,0,0 is the fact that we are now in camera space making the origin the position of the camera
			Vec3f origin = {0,0,0};
			Vec3f cameraRay = origin - cameraSpaceVertices[0];
			float dot = Vec3Dot(faceNormal, cameraRay);
			if (dot < 0) continue;
		}

		//Projection
		Triangle projectedTri = {
			.points = {
				Vec4MultMat4(cameraSpaceVertices[0], renderer.projectionMat),
				Vec4MultMat4(cameraSpaceVertices[1], renderer.projectionMat),
				Vec4MultMat4(cameraSpaceVertices[2], renderer.projectionMat)
			},
			.texCoords = {
				{face.aUV.u, face.aUV.v},
				{face.bUV.u, face.bUV.v},
				{face.cUV.u, face.cUV.v}
			}
		};
		Polygon poly = CreatePolygonFromTriangle(projectedTri);

		//Frustum clipping
		ClipPolygonAxisSide(X_AXIS, 1.0, poly);
		ClipPolygonAxisSide(X_AXIS, -1.0, poly);
		ClipPolygonAxisSide(Y_AXIS, 1.0, poly);
		ClipPolygonAxisSide(Y_AXIS, -1.0, poly);
		ClipPolygonAxisSide(Z_AXIS, 1.0, poly);
		ClipPolygonAxisSide(Z_AXIS, -1.0, poly);

		Triangle clippedTris[MAX_NUM_POLY_TRIS];
		int clippedTrisCount = 0;
		CreateTrisFromPolygon(poly, clippedTris, clippedTrisCount);

		for (int t = 0; t < clippedTrisCount; t++) {
			const Triangle &baseTri = clippedTris[t];
			Triangle triToRender;

			for (int i = 0; i < 3; i++) {
				Vec4f screenPoint = GetScreenCoords(
					baseTri.points[i],
					renderer.projectionMat,
					renderer.windowWidth,
					renderer.windowHeight,
					false
				);

				triToRender.points[i] = screenPoint;
			}

			triToRender.texCoords[0] = baseTri.texCoords[0];
			triToRender.texCoords[1] = baseTri.texCoords[1];
			triToRender.texCoords[2] = baseTri.texCoords[2];

			renderer.trisToRender.push_back(triToRender);
		}
		
		// Triangle triToRender;
		// for (int i = 0; i < 3; i++) {
		// 	Vec3f v = cameraSpaceVertices[i];
		// 	//Camera space -> Raster space
		// 	Vec4f projectedVertex = GetScreenCoords(
		// 		v, 
		// 		renderer.projectionMat, 
		// 		renderer.windowWidth, 
		// 		renderer.windowHeight
		// 	);
		//
		// 	triToRender.points[i] = projectedVertex;
		// }
		// Triangle triToRender;
		// triToRender.texCoords[0] = {face.aUV.u, face.aUV.v};
		// triToRender.texCoords[1] = {face.bUV.u, face.bUV.v};
		// triToRender.texCoords[2] = {face.cUV.u, face.cUV.v};
		//
		// renderer.trisToRender.push_back(triToRender);
	}

	//Time passed between last and this frame. (Converted from ms to seconds)
	renderer.deltaTime = (SDL_GetTicks64() - renderer.msPassedUntilLastFrame) / 1000.0f;
	renderer.msPassedUntilLastFrame = SDL_GetTicks64();
}

void Render() {
	DrawGrid(10);

	for (Triangle &tri : renderer.trisToRender) {
		switch (renderer.renderMode) {
		case RenderMode::NO_TEXTURE: break;
		case RenderMode::FILLED: DrawFilledTriangle(tri, 0xFFFFFFFF); break;
		case RenderMode::TEXTURED: DrawTexturedTriangle(tri, model.meshTexture);
		}

		if (renderer.renderWireframe) { DrawTriangle(tri, 0xFF00FF00); }
	}

	renderer.trisToRender.clear();

	RenderColorBuffer();

	RunImGui(
		renderer.sdlRenderer,
		renderer.rotation,
		renderer.showcase,
		renderer.renderMode,
		renderer.renderWireframe,
		renderer.backfaceCulling
	);

	SDL_RenderPresent(renderer.sdlRenderer);
	ClearColorBuffer(0xFF000000); //Clear with black
	ClearZBuffer();
}

void CleanUp() {
	UnloadObjFile(model.mesh);
	UnloadPngTexture(model);
	delete[] display.colorBuffer;
	delete[] display.zBuffer;
	SDL_DestroyTexture(renderer.sdlColorBufferTexture);
	SDL_DestroyRenderer(renderer.sdlRenderer);
	SDL_DestroyWindow(renderer.sdlWindow);
	SDL_Quit();
}
