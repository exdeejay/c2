#include <iostream>
#include <string>
#include <vector>
#include <portaudio.h>
#include <rigtorp/SPSCQueue.h>
#include <opus.h>
#include "commands.h"
#include "simplepacket.h"
#include "controller.h"
#include "log.h"
using namespace std;

#define CHECK_ERR(fn) status = fn; if (status != paNoError) { ctrl.err_println(Pa_GetErrorText(status)); return status; }

const int audioQueueSize = 0x1000;

AudioCommand::AudioCommand() : audioQueue(audioQueueSize) {}
AudioCommand::~AudioCommand() {
	//TODO: end audio thread and stop stream
}



int audioCallback(
		const void* input, void* output,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
		void* userData) {
	const int16_t* cInput = (const int16_t*) input;
	rigtorp::SPSCQueue<int16_t>* queue = (rigtorp::SPSCQueue<int16_t>*) userData;
	for (size_t i = 0; i < frameCount; i++) {
		bool success = queue->try_push(cInput[i]);
	}
	return paContinue;
}

void flushAudio(Controller* ctrl, rigtorp::SPSCQueue<int16_t>* queue, bool* enabled) {
	int status;
	//TODO: check status
	OpusEncoder* enc = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &status);
	int16_t buf[960];
	const size_t buflen = sizeof(buf)/sizeof(int16_t);
	while (true) {
		while (queue->size() < buflen) {
			if (!*enabled) {
				break;
			}
		}
		if (!*enabled) {
			break;
		}
		for (int i = 0; i < buflen; i++) {
			buf[i] = *queue->front();
			queue->pop();
		}
		uint8_t opusbuf[4000];
		auto len = opus_encode(enc, buf, buflen, opusbuf, 4000);
		slice<uint8_t> opus_slice(opusbuf, len);
		SimplePacket<slice<uint8_t>> audioPacket(4, opus_slice);
		ctrl->send_packet(audioPacket);
	}
	opus_encoder_destroy(enc);
}

int AudioCommand::execute(Controller& ctrl, AudioCommandEnum cmd) {
	if (cmd == AudioCommandEnum::start) {
		if (stream != nullptr) {
			return -1;
		}

		int status;

		enabled = true;
		worker = std::thread(flushAudio, &ctrl, &audioQueue, &enabled);
		CHECK_ERR(Pa_Initialize());
		CHECK_ERR(Pa_OpenDefaultStream(&stream, 1, 0, paInt16, 44100, paFramesPerBufferUnspecified, audioCallback, &audioQueue));
		CHECK_ERR(Pa_StartStream(stream));
		DEBUGLOG("Audio stream started\n");
	} else if (cmd == AudioCommandEnum::stop) {
		if (stream == nullptr) {
			return -1;
		}
		int status;
		CHECK_ERR(Pa_StopStream(stream));
		CHECK_ERR(Pa_CloseStream(stream));
		enabled = false;
		worker.join();
		while (!audioQueue.empty()) {
			audioQueue.pop();
		}
		stream = nullptr;
		CHECK_ERR(Pa_Terminate());
		DEBUGLOG("Stopped audio stream\n");
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
