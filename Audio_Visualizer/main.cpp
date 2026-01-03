#include "audio.h"
#include "fft.h"
#include "visualizer.h"
#include <iostream>

int main() {
	
	std::vector<float> audio_data = {}; 
	
	initLoopback(1000000); 

	recordAudio(30, audio_data); 
	std::cout << "record audio\n"; 

	for (float datum : audio_data) {
		std::cout << datum << "\n"; 
	}

	destroyAudio(); 

	return 0; 
}