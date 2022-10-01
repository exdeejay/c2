#include <iostream>
#include <string>
#include <vector>
#include <portaudio.h>
#include "commands.h"
#include "simplepacket.h"
#include "controller.h"
#include "rigtorp/SPSCQueue.h"
using namespace std;

#define CHECK_ERR(fn) status = fn; if (status != paNoError) { ctrl.err_println(Pa_GetErrorText(status)); return status; }

PaStream* stream = nullptr;
const int audioQueueSize = 0x1000;

AudioCommand::AudioCommand() : audioQueue(audioQueueSize) {}
AudioCommand::~AudioCommand() {
	//TODO: end audio thread
}

int audioCallback(
		const void* input, void* output,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
		void* userData) {
	const uint8_t* cInput = (const uint8_t*) input;
	rigtorp::SPSCQueue<uint8_t>* queue = (rigtorp::SPSCQueue<uint8_t>*) userData;
	for (size_t i = 0; i < frameCount * sizeof(uint16_t); i++) {
		queue->try_push(cInput[i]);
	}
	return paContinue;
}

void flushAudio(Controller* ctrl, rigtorp::SPSCQueue<uint8_t>* queue) {
	vector<uint8_t> buf;
	buf.resize(1024);
	auto iter = buf.begin();
	auto end = buf.cend();
	while (true) {
		if (queue->empty()) {
			continue;
		}
		*iter++ = *queue->front();
		queue->pop();
		if (iter == end) {
			SimplePacket<vector<uint8_t>> audioPacket(4, buf);
			ctrl->send_packet(audioPacket);
			iter = buf.begin();
		}
	}
}

int AudioCommand::execute(Controller& ctrl, AudioCommandEnum cmd) {
	if (cmd == AudioCommandEnum::start) {
		if (stream != nullptr) {
			return -1;
		}

		int status;

		worker = std::thread(flushAudio, &ctrl, &audioQueue);
		CHECK_ERR(Pa_Initialize());
		CHECK_ERR(Pa_OpenDefaultStream(&stream, 1, 0, paInt16, 16000, 256, audioCallback, &audioQueue));
		CHECK_ERR(Pa_StartStream(stream));
	} else if (cmd == AudioCommandEnum::stop) {
		if (stream == nullptr) {
			return -1;
		}
		int status;
		CHECK_ERR(Pa_StopStream(stream));
		CHECK_ERR(Pa_CloseStream(stream));
		stream = nullptr;
		CHECK_ERR(Pa_Terminate());
	} else if (cmd == AudioCommandEnum::list) {
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
