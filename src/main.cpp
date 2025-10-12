//////////////////////////////////////////////////
// Conventions
//////////////////////////////////////////////////
/* 
 *  - Left handed coordinate system
 *  - +Z in to the screen
 *  - Clock-wise winding order
 *  - Row major vectors
 */
//////////////////////////////////////////////////
// Graphics Pipeline
//////////////////////////////////////////////////
/*
 *		Model Space
 *			 |
 *			 v
 *		World Space
 *			 |
 *			 v
 *		Camera Space
 *			 |
 *			 v
 *	  Backface Culling
 *			 |
 *			 v
 *			NDC
 *			 |
 *			 v
 *		Raster Space
 */
//////////////////////////////////////////////////
#include "renderer.h"

bool isRunning = false;

int main(int arc, char* argv[]) {
	isRunning = InitWindow();

	Setup();
	while(isRunning) {
		ProcessInput(isRunning);
		Update();
		Render();
	}
	CleanUp();

	return 0;
}
