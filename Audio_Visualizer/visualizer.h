#pragma once

#include <SDL3/SDL.h>
#include <vector>

void bin_8_avg(int bin[], float data[], size_t n);

void bin_n_avg(float bin[], int n, float data[], size_t size); 

void drawBars(SDL_Renderer* renderer, int sideLength, int arr[], int size); 

void visualize(int sideLength, int bin[], int size); 

void visualizeFFT(int sideLength, float data[], size_t size); 

void visualizer_init(int width, int height); 

void visualizerExit(bool& state); 

