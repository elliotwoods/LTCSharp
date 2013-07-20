// LTCSharp.h

#pragma once
#include "ltc.h"
 
using namespace System;

namespace LTCSharp {

	public enum class TVStandard
	{
		TV525_60i,
		TV625_50i,
		TV1125_60i,
		FILM24p
	};

	public enum class BGFlags
	{
		USE_DATE,
		TC_CLOCK,
		BGF_DONT_TOUCH,
		NO_PARITY
	};

	public ref class Timecode
	{
	public:
		Timecode();
		~Timecode();

		SMPTETimecode * getInstance() { return this->instance; };
	protected:
		SMPTETimecode * instance;
	};

	public ref class Frame
	{
	public:
		Frame();
		~Frame();

		LTCFrameExt * getInstance() { return this->instance; }
	protected:
		LTCFrameExt * instance;
	};

	public ref class Utils {
	public:
		static LTC_TV_STANDARD toNative(TVStandard standard);
		static int toNative(BGFlags flags);
	};

	public ref class Decoder {
	public:
		Decoder(int approxAudioSampleRate, int approxFrameRate, int queueSize);
		~Decoder();

		void write(IntPtr buffer, TypeCode type, int size, int offset);
		void write(array<System::Byte>^ buffer, int offset); 
		void write(array<short>^ buffer, int offset);
		void write(array<unsigned short>^ buffer, int offset);
		void write(array<float>^ buffer, int offset);

		void flushQueue();
		int getQueueLength();
		Frame^ read();

	protected:
		LTCDecoder * instance;
	};

	public ref class Encoder
	{
	public:
		Encoder(double sampleRate, double fps, TVStandard standard, BGFlags flags);
		~Encoder();

		void setTimecode(Timecode ^ timecode);
		Timecode ^ getTimecode();
		
		void setFrame(Frame ^ frame);
		Frame ^ getFrame();

		void incrementFrame();
		void decrementFrame();

		int getBuffer(array<Byte>^ buffer, int offset);
		IntPtr getBufferPointer();
		void flushBuffer();

		void setBufferSize(double sampleRate, double fps);
		void setVolume(double deciBelFullScale);

		void encodeFrame();
	protected:
		LTCEncoder * instance;
	};
}
