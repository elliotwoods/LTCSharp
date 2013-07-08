// This is the main DLL file.

#include "stdafx.h"

#include "Decoder.h"

namespace LTCSharp
{
	//----------
	Decoder::Decoder(int approxAudioSampleRate, int approxFrameRate, int queueSize) {
		this->instance = ltc_decoder_create(approxAudioSampleRate / approxFrameRate);
	}

	//----------
	Decoder::~Decoder() {
		ltc_decoder_free(this->instance);
	}

	//----------
	void Decoder::flush() {
		ltc_decoder_queue_flush(this->instance);
	}

	//----------
	int Decoder::getQueueLength() {
		return ltc_decoder_queue_length(this->instance);
	}
}