// LTCSharp.h

#pragma once
#include "ltc.h"
 
using namespace System;
using namespace System::Runtime::InteropServices;

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
		NONE,
		USE_DATE,
		TC_CLOCK,
		BGF_DONT_TOUCH,
		NO_PARITY
	};

	public ref class Timecode : public IFormattable
	{
	public:
		Timecode();
		Timecode(String ^ timezone, int years, int months, int days, int hours, int minutes, int seconds, int frame);
		Timecode(int hours, int minutes, int seconds, int frame);

		~Timecode();

		SMPTETimecode * getInstance() { return this->instance; };
		
		property int Years { int get() { return this->getInstance()->years; } }
		property int Months { int get() { return this->getInstance()->months; } }
		property int Days { int get() { return this->getInstance()->days; } }
		property int Hours { int get() { return this->getInstance()->hours; } }
		property int Minutes { int get() { return this->getInstance()->mins; } }
		property int Seconds { int get() { return this->getInstance()->secs; } }
		property int Frame { int get() { return this->getInstance()->frame; } }

		virtual String^ ToString() override;
		virtual String^ ToString(String^ format) override;
		virtual String^ ToString(String^ format, IFormatProvider^ provider) override;

	protected:
		SMPTETimecode * instance;
	};

	public ref class Frame
	{
	public:
		Frame();
		~Frame();

		Timecode^ getTimecode();

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

		void Write(IntPtr buffer, TypeCode type, int size, int offset);
		void Write(array<System::Byte>^ buffer, int count, int offset); 
		void Write(array<short>^ buffer, int count, int offset);
		void Write(array<unsigned short>^ buffer, int count, int offset);
		void Write(array<float>^ buffer, int count, int offset);

		void WriteAsU16(array<System::Byte>^ buffer, int count, int offset);
		void WriteAsS16(array<System::Byte>^ buffer, int count, int offset);
		void WriteAsFloat(array<System::Byte>^ buffer, int count, int offset);

		void FlushQueue();
		int GetQueueLength();
		Frame^ Read();

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
