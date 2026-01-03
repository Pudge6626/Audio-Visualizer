#include "visualizer.h"

static SDL_Window* window = NULL; 
static SDL_Renderer* renderer = NULL; 
static int windowLength; 

SDL_Event event = {}; 



void bin_8_avg(int bin[], std::vector<float> data, size_t n) {

	int l = n / 8;
	float avg = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = i * l; j < (i + 1) * l; j++) {
			avg += data.at(j);
		}
		bin[i] = round(avg / n);
		avg = 0;
	}

}


//draws out the bars (audio visualization) of an array of amplitudes
void drawBars(SDL_Renderer* renderer, int sideLength, int arr[], int size) {
	//spacing size between squares
	int s = sideLength / 2;
	//gets number of bars to draw 
	int barNum = size;
	//total length of bars including spacing size
	int L = (barNum * sideLength) + ((barNum - 1) * s);
	//space from beginning bar and end bar to the edge of the window, used for centering
	int r = round((windowLength - L) / 2.0);
	//starts by examining each bar
	for (int i = 0; i < barNum; i++) {
		//creates list of squares corresponding to amplitude of one bar
		SDL_FRect* rects = (SDL_FRect*)malloc(arr[i] * sizeof(SDL_FRect));
		//sets structure parameters for squares
		for (int j = 0; j < arr[i]; j++) {
			rects[j].w = sideLength;
			rects[j].h = sideLength;
			rects[j].x = r + (i * (sideLength + s)); //centers the bars
			rects[j].y = windowLength - ((j + 1) * (sideLength + s));
		}
		//renderers squares
		SDL_RenderFillRects(renderer, rects, arr[i]);
		free(rects); //dynamic array; new array for each bar
	}
}


void visualize(int sideLength, int bin[], int size) {

	//draws black screen to reset
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer); 
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

	//tells the renderer to draw the amplitude bars 
	drawBars(renderer, sideLength, bin, size); 

	//presents drawing
	SDL_RenderPresent(renderer); 


}

void visualizer_init(int length) {

	windowLength = length; 

	//if sdl cant initialize we log the problem and return a failure to halt the callback
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Couldnt Initialilze SDL: %s", SDL_GetError());

	}


	//is the window or referer fails to be created, throw an error
	if (!SDL_CreateWindowAndRenderer("Audio Visualizer", windowLength, windowLength, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Coulnt Create Window or Renderer: %s", SDL_GetError());

	}

	//sets render parameters when it comes to the resizing of the window 
	SDL_SetRenderLogicalPresentation(renderer, windowLength, windowLength, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	


}

//modifies state boolean when the window is exited 
void visualizerExit(bool& state) {

	while (SDL_PollEvent(&event)) {

		if (event.type == SDL_EVENT_QUIT) {
			state = false; 
			SDL_DestroyRenderer(renderer); 
			SDL_DestroyWindow(window); 
		}

	}

}

