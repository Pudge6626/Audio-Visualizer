#include "audio.h"
#include "fft.h"
#include "visualizer.h"
#include <iostream>

//20ms of buffer
#define REQUESTED_DURATION 200000 

//will record 10ms of audio
#define SLEEP_TIME_MS 10

//500 pixel box window
#define WINDOW_SIZE 500

//N size array for drawing
#define N 30 

int main() {

	initLoopback(REQUESTED_DURATION);
	std::vector<float> myAudio = {};
	float* freqSpectrum;
	visualizer_init(1920, 600);
	bool state = true;
	int c = 0;

	while (state) {

		recordAudio(SLEEP_TIME_MS, myAudio);
		size_t size = myAudio.size();

		if (size != 0) {
			freqSpectrum = (float*)malloc(size * sizeof(float));
			fft(myAudio, freqSpectrum, size);
			visualizeFFT(2, freqSpectrum, size);
			myAudio.clear();
			free(freqSpectrum);

		}

		//std::cout << "size: " << size << "\n"; 
		visualizerExit(state);
		Sleep(5); 

	
	}

	destroyAudio(); 
	return 0; 
} 