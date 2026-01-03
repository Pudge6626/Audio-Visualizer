#pragma once

#include <SDL3/SDL.h>
#include <vector>

void bin_8_avg(int bin[], std::vector<float> data, size_t n); 

void drawBars(SDL_Renderer* renderer, int sideLength, int arr[], int size); 

void visualize(int sideLength, int bin[], int size); 

void visualizer_init(int length); 

void visualizerExit(bool& state); 

