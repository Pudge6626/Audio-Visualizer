#define SDL_MAIN_USE_CALLBACKS  1; 
#include <SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include <iostream>
#include <cmath>
#include <windows.h>
#include <fftw3.h>
#include <vector>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <cassert>

//create window and renderer pointers, these handle their respective tasks
static SDL_Window* window = NULL; 
static SDL_Renderer* renderer = NULL;

#define windowHeight 500
#define squareLength 20
#define amplitude 8
#define N 2048  //2^11
#define REAL 0
#define IMAGINARY 1
#define REFTIME_REQUESTED 1000000 //100 ms
#define SLEEP_TIME_MS 30 
#define SAFE_RELEASE(punk) if ((punk) != NULL) {(punk)->Release(); (punk) = NULL;}


using namespace std; 

fftw_complex* in, * out; //global input and output arrays
fftw_plan p; //global plan for FFT 
float* drawArr; 
int bin_8[8]; 

// sets these variables equal to the GUID representation of these interfaces/classes
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);


HRESULT hr;
REFERENCE_TIME hnsRequestedDuration = REFTIME_REQUESTED;
REFERENCE_TIME hnsActualDuration;
UINT32 bufferFrameCount;
UINT32 numFramesAvailable;
IMMDeviceEnumerator* pEnumerator = NULL;
IMMDevice* pDevice = NULL;
IAudioClient* pAudioClient = NULL;
IAudioCaptureClient* pCaptureClient = NULL;
WAVEFORMATEX* pwfx = NULL;
UINT32 packetLength = 0;
BOOL bDone = NULL;
BYTE* pData;
DWORD flags;
vector<float> myData = {};


void copyToInput(fftw_complex * x, vector<float> y, size_t l) { 
	
	for (int i = 0; i < l; i++) {
		x[i][REAL] = y.at(i); 

	}


}


void copyToDraw(float* x, fftw_complex* y, size_t n) {
	x = (float*)malloc(n * sizeof(float)); 
	for (int i = 0; i < n; i++) {
		x[i] = y[i][REAL];
	}

}

void bin_8_avg(int* bin, float* data, size_t n) {
	int l = n / 8; 
	float avg = 0; 
	for (int i = 0; i < 8; i++) {
		for (int j = i * l; j < (i + 1) * l; j++) {
			avg += data[j]; 
		}
		bin[i] = round(avg / n); 
		avg = 0; 
	}


}


//draws out the bars (audio visualization) of an array of amplitudes
void drawBar(SDL_Renderer* renderer, int sideLength, int arr[], int size) {
	//spacing size between squares
	int s = sideLength / 2; 
	//gets number of bars to draw 
	int barNum = size;
	//total length of bars including spacing size
	int L = (barNum * sideLength) + ((barNum - 1) * s); 
	//space from beginning bar and end bar to the edge of the window, used for centering
	int r = round((windowHeight -  L) / 2.0); 
	//starts by examining each bar
	for (int i = 0; i < barNum; i++) {
		//creates list of squares corresponding to amplitude of one bar
		SDL_FRect* rects = (SDL_FRect*)malloc(arr[i] * sizeof(SDL_FRect));
		//sets structure parameters for squares
		for (int j = 0; j < arr[i]; j++) {
			rects[j].w = sideLength;
			rects[j].h = sideLength;
			rects[j].x = r + (i * (sideLength + s)); //centers the bars
			rects[j].y = windowHeight - ((j + 1) * (sideLength + s)); 
		}
		//renderers squares
		SDL_RenderFillRects(renderer, rects, arr[i]);
		free(rects); //dynamic array; new array for each bar
	} 
}







//callback initialization of SDL
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED); 
	assert(SUCCEEDED(hr)); 
	//if sdl cant initialize we log the problem and return a failure to halt the callback
	if (!SDL_INIT_VIDEO) {
		SDL_Log("Couldnt Initialilze SDL: %s", SDL_GetError()); 
		return SDL_APP_FAILURE; 

	}


	//is the window or referer fails to be created, throw an error
	if (!SDL_CreateWindowAndRenderer("Audio Visualizer", windowHeight, windowHeight, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("Coulnt Create Window or Renderer: %s", SDL_GetError()); 

		return SDL_APP_FAILURE; 

	}

	//sets render parameters when it comes to the resizing of the window 
	SDL_SetRenderLogicalPresentation(renderer, windowHeight, windowHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX); 
	cout << "success\n"; 
	

	
	//creates single instance of device enumerator
	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator);
	assert(SUCCEEDED(hr));

	//writes address of default capture audio endpoint to pDevice pointer
	hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
	assert(SUCCEEDED(hr));

	//uses device interface function to create audio client
	hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
	assert(SUCCEEDED(hr));

	//gets format of audio stream
	hr = pAudioClient->GetMixFormat(&pwfx);
	assert(SUCCEEDED(hr));

	//initializes audio stream 
	hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, hnsRequestedDuration, 0, pwfx, NULL);
	assert(SUCCEEDED(hr));

	//gets size of allocated buffer, useful for calculations
	hr = pAudioClient->GetBufferSize(&bufferFrameCount);
	assert(SUCCEEDED(hr));

	//expands functionality of audio client
	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
	assert(SUCCEEDED(hr));

	
	return SDL_APP_CONTINUE; 

}


//when you exit out of the window it will return app success
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {

	if (event->type == SDL_EVENT_QUIT) {

		cout << "In: "; 
		for (int i = 0; i < N; i++) {
			printf("%d, ", (int)in[i][REAL]); 

		}

		cout << "\nOut: "; 
		
		for (int i = 0; i < N; i++) {
			printf("%d, ", (int)out[i][REAL]); 
		}

		fftw_destroy_plan(p); 
		fftw_free(in);  
		fftw_free(out);  
		CoTaskMemFree(pwfx); 
		SAFE_RELEASE(pEnumerator); 
		SAFE_RELEASE(pDevice); 
		SAFE_RELEASE(pAudioClient); 
		SAFE_RELEASE(pCaptureClient); 


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
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);  
		
		//start recording, will fill up buffer
		hr = pAudioClient->Start();
		assert(SUCCEEDED(hr));

		//sleep for defined length; 30ms
		Sleep(SLEEP_TIME_MS);

		//stop recording
		hr = pAudioClient->Stop();
		assert(SUCCEEDED(hr));
		
		//get initial size of packet 
		hr = pCaptureClient->GetNextPacketSize(&packetLength); 
		assert(SUCCEEDED(hr)); 


		while (packetLength != 0) {

			//retrieve packet and store it in pData
			hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
			assert(SUCCEEDED(hr));

			//reinterprets BYTE array as float array
			float* float_data = (float*)pData; 

			//compute number of floats stored in pData
			int float_length = (pwfx->nChannels * pwfx->wBitsPerSample * numFramesAvailable) / 32; 

			//if silent audio record zeros, one channel 
			if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {

				for (int i = 0; i < float_length; i += 2) {

					myData.push_back(0); 
				}

			}

			else {

				for (int i = 0; i < float_length; i += 2)  {

					//store in myData the average of the two channels
					myData.push_back((*(float_data + i) + *(float_data + i + 1)) / 2.);

				}

			}

		}

		int size = myData.size();  

		in = (fftw_complex*)fftw_malloc(size * sizeof(fftw_complex)); 
		out = (fftw_complex*)fftw_malloc(size * sizeof(fftw_complex)); 
		//quickly made computation plan
		p = fftw_plan_dft_1d(size, in, out, FFTW_FORWARD, FFTW_ESTIMATE); 


		//copy vector data to input for FFT
		copyToInput(in, myData, size); 

		//performs fft
		fftw_execute(p); 
		
		//copies the non-junk FFT output 
		copyToDraw(drawArr, out, size); 

		//averages out drawArr into 8 bins for a size 8 array
		bin_8_avg(bin_8, drawArr, size); 

		//draws limited fft output
		drawBar(renderer, squareLength, bin_8, amplitude); 
		//note: can render within a function then present outside the function 
		SDL_RenderPresent(renderer);
		return SDL_APP_CONTINUE; 
	}



    //runs in the background via SDL, it will close the application once App Success is reached 
	void SDL_AppQuit(void* appstate, SDL_AppResult result) {


	}