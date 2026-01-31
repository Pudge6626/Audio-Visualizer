#include "fft.h"
#include <cassert>

#define REAL 0 
#define IMAGINARY 1


void fft(std::vector<float> input, float output[], int n) {
	
	//input and output to fftw functions 
	fftw_complex* in = (fftw_complex*)fftw_malloc(n * sizeof(fftw_complex)); 
	fftw_complex* out = (fftw_complex*)fftw_malloc(n * sizeof(fftw_complex)); 

	//creates plan of execution quickly 
	fftw_plan plan = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_ESTIMATE); 

	

	//copy input vector into input fftw array
	for (int i = 0; i < n; i++) {

		assert(std::isfinite(input.at(i)));

		in[i][REAL] = input.at(i); 
		in[i][IMAGINARY] = 0.0f;

	}

	fftw_execute(plan); 

	for (int i = 0; i < n; i++) {
		//float temp = sqrt(pow(out[i][REAL], 2) + pow(out[i][IMAGINARY], 2)); 
		float temp = sqrt(out[i][REAL] * out[i][REAL] + out[i][IMAGINARY] * out[i][IMAGINARY]);
	

		assert(std::isfinite(temp)); 

		//assert(temp > 0); 
		output[i] = temp; 

	}

	fftw_free(in); 
	fftw_free(out); 
	fftw_destroy_plan(plan); 
}