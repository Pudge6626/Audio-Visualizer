#include "fft.h"

#define REAL 0 
#define IMAGINARY 1


void fft(std::vector<float> input, float output[], size_t n) {
	
	//input and output to fftw functions 
	fftw_complex* in = (fftw_complex*)fftw_malloc(n * sizeof(fftw_complex)); 
	fftw_complex* out = (fftw_complex*)fftw_malloc(n * sizeof(fftw_complex)); 

	//creates plan of execution quickly 
	fftw_plan plan = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_MEASURE); 

	//copy input vector into input fftw array
	for (int i = 0; i < n; i++) {

		in[i][REAL] = input.at(i); 

	}

	fftw_execute(plan); 

	for (int i = 0; i < n; i++) {
		float temp = sqrt(pow(out[i][REAL], 2) + pow(out[i][IMAGINARY], 2)); 
		output[i] = sqrt( pow(out[i][REAL], 2) + pow(out[i][IMAGINARY], 2)); 

	}

	fftw_free(in); 
	fftw_free(out); 
	fftw_destroy_plan(plan); 
}