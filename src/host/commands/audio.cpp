#include <iostream>
#include <string>
#include <vector>
#include <portaudio.h>
#include "basecommand.h"
using namespace std;

#define CHECK_ERR(fn) status = fn; if (status != paNoError) { ctrl.err_println(Pa_GetErrorText(status)); return status; }

PaStream* stream = nullptr;

int audioCallback(
		const void* input, void* output,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
		void* userData) {
	const char* cInput = (const char*) input;
	Controller* ctrl = (Controller*) userData;
	ctrl->buffer_audio(cInput, frameCount * sizeof(uint16_t));
	return paContinue;
}

int audioCommand(Controller& ctrl, AudioCommand cmd) {
	if (cmd == AudioCommand::start) {
		if (stream != nullptr) {
			return -1;
		}

		int status;
		CHECK_ERR(Pa_Initialize());
		CHECK_ERR(Pa_OpenDefaultStream(&stream, 1, 0, paInt16, 16000, 256, audioCallback, &ctrl));
		CHECK_ERR(Pa_StartStream(stream));
	} else if (cmd == AudioCommand::stop) {
		if (stream == nullptr) {
			return -1;
		}
		int status;
		CHECK_ERR(Pa_StopStream(stream));
		CHECK_ERR(Pa_CloseStream(stream));
		stream = nullptr;
		CHECK_ERR(Pa_Terminate());
	} else if (cmd == AudioCommand::list) {
		int status;
		
		CHECK_ERR(Pa_Initialize());
		int numDevices = Pa_GetDeviceCount();
		if (numDevices < 0) {
			CHECK_ERR(numDevices);
		}

		string message;
		const PaDeviceInfo* devInfo;
		int defaultIn = Pa_GetDefaultInputDevice();
		int defaultOut = Pa_GetDefaultOutputDevice();
		for (int i = 0; i < numDevices; i++) {
			devInfo = Pa_GetDeviceInfo(i);
			message += to_string(i);
			message += ": ";
			message += devInfo->name;
			message += " ";
			message += to_string(devInfo->maxInputChannels) + ":" + to_string(devInfo->maxOutputChannels);
			if (i == defaultIn || i == defaultOut) {
				message += " (*)";
			}
			message += "\n";

		}
		ctrl.println(message);

		CHECK_ERR(Pa_Terminate());
	}

	return 0;
}
