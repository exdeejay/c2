#include "commands.h"

#include <iostream>
#include <string>
#include <vector>
#include <portaudio.h>


#define CHECK_ERR(fn) status = fn; if (status != paNoError) { ctrl->sendErr(Pa_GetErrorText(status)); return status; }

PaStream* stream = nullptr;

int audioCallback(
		const void* input, void* output,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
		void* userData) {
	const char* cInput = (const char*) input;
	Controller* ctrl = (Controller*) userData;
	//ctrl->bufferAudio(cInput, frameCount * 2);
	return paContinue;
}

int audioCommand(Controller* ctrl, AudioCommand cmd) {
	if (cmd == AudioCommand::start) {
		if (stream != nullptr) {
			return -1;
		}

		int status;
		//CHECK_ERR(Pa_Initialize());
		//CHECK_ERR(Pa_OpenDefaultStream(&stream, 1, 0, paInt16, 16000, 256, audioCallback, ctrl));
		//CHECK_ERR(Pa_StartStream(stream));
	} else if (cmd == AudioCommand::stop) {
		if (stream == nullptr) {
			return -1;
		}
		int status;
		//CHECK_ERR(Pa_StopStream(stream));
		//CHECK_ERR(Pa_CloseStream(stream));
		stream = nullptr;
		//CHECK_ERR(Pa_Terminate());
	} else if (cmd == AudioCommand::list) {
		int status;
		
		//CHECK_ERR(Pa_Initialize());
		int numDevices = Pa_GetDeviceCount();
		if (numDevices < 0) {
			//CHECK_ERR(numDevices);
		}

		std::string message;
		const PaDeviceInfo* devInfo;
		int defaultIn = Pa_GetDefaultInputDevice();
		int defaultOut = Pa_GetDefaultOutputDevice();
		for (int i = 0; i < numDevices; i++) {
			devInfo = Pa_GetDeviceInfo(i);
			message += std::to_string(i);
			message += ": ";
			message += devInfo->name;
			message += " ";
			message += std::to_string(devInfo->maxInputChannels) + ":" + std::to_string(devInfo->maxOutputChannels);
			if (i == defaultIn || i == defaultOut) {
				message += " (*)";
			}
			message += "\n";

		}
		//ctrl->sendOut(message);

		//CHECK_ERR(Pa_Terminate());
	}

	return 0;
}
