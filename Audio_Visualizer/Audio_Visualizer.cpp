#define SDL_MAIN_USE_CALLBACKS  1; 
#include <SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include <iostream>
#include <vector>

//create window and renderer pointers, these handle their respective tasks
static SDL_Window* window = NULL; 
static SDL_Renderer* renderer = NULL; 

#define windowWidth 640
#define windowHeight 480
#define squareLength 25
#define amplitude 10; 

using namespace std; 


void drawBar(SDL_Renderer* renderer, int sideLength, int arr[]) {
	int barNum = sizeof(arr) / sizeof(*arr); 
	for (int i = 0; i < barNum; i++) {
		SDL_FRect* rects = (SDL_FRect*)malloc(arr[i] * sizeof(SDL_FRect));
		for (int j = 0; j < arr[i]; j++) {
			rects[j].w = sideLength;
			rects[j].h = sideLength;
			rects[j].x = (50 * i);
			rects[j].y = (50 * j);

		}
		SDL_RenderFillRects(renderer, rects, arr[i]);
		free(rects);
	}
	//SDL_RenderPresent(renderer); 
}







//callback initialization of SDL
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	//if sdl cant initialize we log the problem and return a failure to halt the callback
	if (!SDL_INIT_VIDEO) {
		SDL_Log("Couldnt Initialilze SDL: %s", SDL_GetError()); 
		return SDL_APP_FAILURE; 

	}

	//is the window or referer fails to be created, throw an error
	if (!SDL_CreateWindowAndRenderer("title", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Coulnt Create Window or Renderer: %s", SDL_GetError()); 

		return SDL_APP_FAILURE; 

	}

	//sets render parameters when it comes to the resizing of the window 
	SDL_SetRenderLogicalPresentation(renderer, windowWidth, windowHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX); 
	cout << "success\n"; 
	return SDL_APP_CONTINUE; 

}


//when you exit out of the window it will return app success
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

	if (event->type == SDL_EVENT_QUIT) {

		return SDL_APP_SUCCESS;
	}

	else {
		return SDL_APP_CONTINUE;
	}

}
	
   // Continuous iteration, this test code draws a simple blue square 
   SDL_AppResult SDL_AppIterate(void* appstate) {
		SDL_FRect rectangle, test;  
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
		SDL_RenderClear(renderer); //draws black background using renderer, repeats for every new drawing

		//creates a 50x50 square at position (400,50) 
		rectangle.w = 50; 
		rectangle.h = 50; 
		rectangle.x = 400; 
		rectangle.y = 50; //top of rectangle is this many pixels from top of the screen
/*
		test.w = 50;  
		test.h = 50; 
		test.x = 50; 
		test.y = 50; 

		*/ 

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE); 
		SDL_RenderFillRect(renderer, &rectangle); 
		int arr[] = { 3,2,1 }; 
		int sideLength = 25; 
		int barNum = sizeof(arr) / sizeof(*arr);

		drawBar(renderer, 10, arr); 


		
		//SDL_RenderPresent(renderer);
		
		
		//bool b = SDL_RenderFillRect(renderer, &test); 
		//printf("%i\n", b); 

		//draws a solid blue rectangle
		SDL_RenderPresent(renderer); 
		return SDL_APP_CONTINUE; 
	}



    //runs in the background via SDL, it will close the application once App Success is reached 
	void SDL_AppQuit(void* appstate, SDL_AppResult result) {


	}