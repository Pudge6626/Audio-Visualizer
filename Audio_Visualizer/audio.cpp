#include "audio.h"


#define SAFE_RELEASE(punk) if ((punk) != NULL) {(punk)->Release(); (punk) = NULL;}

// sets these variables equal to the GUID representation of these interfaces/classes
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

//global variables
HRESULT hr;
UINT32 numFramesAvailable;
IMMDeviceEnumerator* pEnumerator = NULL;
IMMDevice* pDevice = NULL;
IAudioClient* pAudioClient = NULL;
IAudioCaptureClient* pCaptureClient = NULL;
WAVEFORMATEX* pwfx = NULL;
UINT32 packetLength = 0;
BYTE* pData; 
DWORD flags;



//initializes loopback stream of requested size in time units 
void initLoopback(REFERENCE_TIME hnsRequestedDuration) {

	//initialize COM library
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED); 
	assert(SUCCEEDED(hr)); 

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

	//expands functionality of audio client
	hr = pAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient);
	assert(SUCCEEDED(hr));
}

void recordAudio(DWORD sleep_time_ms, std::vector<float> &audio_data) {

	//start recording for sleep_time_ms long
	hr = pAudioClient->Start(); 
	assert(SUCCEEDED(hr)); 

	Sleep(sleep_time_ms); 

	pAudioClient->Stop(); 
	assert(SUCCEEDED(hr)); 

	//get first available packet size
	hr = pCaptureClient->GetNextPacketSize(&packetLength);
	assert(SUCCEEDED(hr));

	while (packetLength != 0) {


		//retrieve packet and store it in pData
		hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL);
		assert(SUCCEEDED(hr));

		

		//reinterpret BYTEs as 32 bit floats 
		float* float_data = (float*)pData;

		//number of floats float_data points to
		int float_length = (pwfx->nChannels * pwfx->wBitsPerSample * numFramesAvailable) / 32; 

		//if there is silence record 1 channel of zeros
		if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {

			for (int i = 0; i < float_length; i += 2) {

				audio_data.push_back(0);

			
			}

		}

		//otherwise average the two channels
		else {

			for (int i = 0; i < float_length; i += 2) {

				audio_data.push_back((*(float_data + i) + *(float_data + i + 1)) / 2.);

			}

		}

		//release packet so that it may extract more data from the buffer
		hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
		assert(SUCCEEDED(hr));

		//get new packet length of the data the packet retrieved 
		hr = pCaptureClient->GetNextPacketSize(&packetLength);
		assert(SUCCEEDED(hr));

	}


}

void destroyAudio() {

	CoTaskMemFree(pwfx); 
	SAFE_RELEASE(pEnumerator); 
	SAFE_RELEASE(pDevice); 
	SAFE_RELEASE(pAudioClient); 
	SAFE_RELEASE(pCaptureClient); 


}