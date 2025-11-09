#define SDL_MAIN_USE_CALLBACKS  1; 
#include <SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include <iostream>
#include <vector>

//create window and renderer pointers, these handle their respective tasks
static SDL_Window* window = NULL; 
static SDL_Renderer* renderer = NULL; 

#define windowWidth 500
#define windowHeight 500
#define squareLength 20
#define amplitude 10; 

using namespace std; 


//draws out the bars (audio visualization) of an array of amplitudes
void drawBar(SDL_Renderer* renderer, int sideLength, int arr[], int size) {
	//spacing size between squares
	int s = 10; 
	//gets number of bars to draw 
	int barNum = size;
	//starts by examining each bar
	for (int i = 0; i < barNum; i++) {
		//creates list of squares corresponding to amplitude of one bar
		SDL_FRect* rects = (SDL_FRect*)malloc(arr[i] * sizeof(SDL_FRect));
		//sets structure parameters for squares
		for (int j = 0; j < arr[i]; j++) {
			rects[j].w = sideLength;
			rects[j].h = sideLength;
			rects[j].x = (i + 1) * (s) + (i * sideLength); 
			rects[j].y = windowHeight - ((j + 1) * (sideLength + s)); 
			SDL_RenderFillRect(renderer, &rects[j]); 
		}
		//renderers squares
		//SDL_RenderFillRects(renderer, rects, arr[i]);
		free(rects); //dynamic array; new array for each bar
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
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
		SDL_RenderClear(renderer); //draws black background using renderer, repeats for every new drawing


		SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  
		//test array
		int arr[] = { 3,2,4,5,2,1,1 }; 
		//calculate size before hand to avoid pointer arithmatic problems
		int size = sizeof(arr) / sizeof(*arr); 
		drawBar(renderer, squareLength, arr, size); 
		//note: can render within a function then present outside the function 
		SDL_RenderPresent(renderer);
		 
		return SDL_APP_CONTINUE; 
	}



    //runs in the background via SDL, it will close the application once App Success is reached 
	void SDL_AppQuit(void* appstate, SDL_AppResult result) {


	}