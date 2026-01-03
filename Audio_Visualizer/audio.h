#pragma once

#include <cassert>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <synchapi.h>
#include <vector>

//initialize audio stream and buffer
void initLoopback(REFERENCE_TIME hnsRequestedDuration); 

//record requested time of audio and stores it in vector 
void recordAudio(DWORD sleep_time_ms, std::vector<float>& audio_data); 

//de allocates memory 
void destroyAudio(); 