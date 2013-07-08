// LTCSharp.h

#pragma once

using namespace System;

namespace LTCSharp {

	public ref class Decoder
	{
	public:
		Decoder(int approxAudioSampleRate, int approxFrameRate, int queueSize);
		~Decoder();

		void write(array<System::Byte>^ buffer, int offset); 
		void write(array<float>^ buffer, int offset); 
		void flush();
		int getQueueLength();

	protected:
		LTCDecoder * instance;
	};
}
