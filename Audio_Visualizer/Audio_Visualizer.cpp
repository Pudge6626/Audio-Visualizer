#define SDL_MAIN_USE_CALLBACKS  1; 
#include <SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include <iostream>


//create window and renderer pointers, these handle their respective tasks
static SDL_Window* window = NULL; 
static SDL_Renderer* renderer = NULL; 

#define windowWidth 640
#define windowHeight 480

using namespace std; 

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
		SDL_FRect rectangle;  
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
		SDL_RenderClear(renderer); 

		rectangle.w = 50; 
		rectangle.h = 50; 
		rectangle.x = 400; 
		rectangle.y = 50; 

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE); 
		SDL_RenderFillRect(renderer, &rectangle); 

		SDL_RenderPresent(renderer); 
		return SDL_APP_CONTINUE; 
	}



    //runs in the background via SDL, it will close the application once App Success is reached 
	void SDL_AppQuit(void* appstate, SDL_AppResult result) {


	}