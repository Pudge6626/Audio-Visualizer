#include "audio.h"
#include "fft.h"
#include "visualizer.h"
#include <iostream>

//20ms of buffer
#define REQUESTED_DURATION 1000000 

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
			//std::cout << "freqSpectrum[0]: " << freqSpectrum[0] << "\n"; 
			visualizeFFT(2, freqSpectrum, size);
			myAudio.clear();
			free(freqSpectrum);

		}

		Sleep(10); 
		visualizerExit(state);
		

	
	}
	 
	destroyAudio(); 
	return 0; 
} 