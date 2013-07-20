// This is the main DLL file.

#include "stdafx.h"

#include "Decoder.h"

namespace LTCSharp
{
#pragma mark Frame
	//----------
	Frame::Frame() {
		this->instance = new LTCFrameExt();
	}

	//----------
	Frame::~Frame() {
		delete(this->instance);
	}

#pragma mark Timecode
	//----------
	Timecode::Timecode() {
		this->instance = new SMPTETimecode();
	}

	//----------
	Timecode::~Timecode() {
		delete this->instance;
	}

#pragma mark Decoder
	//----------
	Decoder::Decoder(int approxAudioSampleRate, int approxFrameRate, int queueSize) {
		this->instance = ltc_decoder_create(approxAudioSampleRate / approxFrameRate, queueSize);
	}

	//----------
	Decoder::~Decoder() {
		ltc_decoder_free(this->instance);
	}

	//----------
	void Decoder::write(IntPtr buffer, TypeCode type, int size, int offset) {
		switch(type)
		{
		case System::TypeCode::Byte:
			ltc_decoder_write(this->instance, (ltcsnd_sample_t*) (void*) buffer, size, offset);
			break;

		case System::TypeCode::UInt16:
			ltc_decoder_write_u16(this->instance, (unsigned short*) (void*) buffer, size, offset);
			break;

		case System::TypeCode::Int16:
			ltc_decoder_write_s16(this->instance, (short*) (void*) buffer, size, offset);
			break;

		case System::TypeCode::Single:
			ltc_decoder_write_float(this->instance, (float*) (void*) buffer, size, offset);
			break;
		};
	}
	
	//----------
	void Decoder::write(array<System::Byte>^ buffer, int offset) {
		pin_ptr<System::Byte> pinned = &buffer[0];
		unsigned char* pointer = pinned;
		ltc_decoder_write(this->instance, pointer, buffer->Length, offset);
	}
	
	//----------
	void Decoder::write(array<short>^ buffer, int offset) {
		pin_ptr<short> pinned = &buffer[0];
		short* pointer = pinned;
		ltc_decoder_write_s16(this->instance, pointer, buffer->Length, offset);
	}
	
	//----------
	void Decoder::write(array<unsigned short>^ buffer, int offset) {
		pin_ptr<unsigned short> pinned = &buffer[0];
		unsigned short* pointer = pinned;
		ltc_decoder_write_u16(this->instance, pointer, buffer->Length, offset);
	}

	//----------
	void Decoder::write(array<float>^ buffer, int offset) {
		pin_ptr<float> pinned = &buffer[0];
		float* pointer = pinned;
		ltc_decoder_write_float(this->instance, pointer, buffer->Length, offset);
	}

	//----------
	void Decoder::flushQueue() {
		ltc_decoder_queue_flush(this->instance);
	}

	//----------
	int Decoder::getQueueLength() {
		return ltc_decoder_queue_length(this->instance);
	}

	//----------
	Frame^ Decoder::read() {
		Frame^ frame = gcnew Frame();
		if (ltc_decoder_read(this->instance, frame->getInstance())) {
			return frame;
		} else {
			throw(gcnew System::Exception("No frames available in queue"));
		}
	}

#pragma mark Encoder
	//----------
	Encoder::Encoder(double sampleRate, double fps, TVStandard standard, BGFlags flags) {
		int flagsDecoded = 0;
		if (flags.HasFlag(BGFlags::USE_DATE))
			flagsDecoded |= LTC_USE_DATE;
		if (flags.HasFlag(BGFlags::TC_CLOCK))
			flagsDecoded |= LTC_TC_CLOCK;
		if (flags.HasFlag(BGFlags::BGF_DONT_TOUCH))
			flagsDecoded |= LTC_BGF_DONT_TOUCH;
		if (flags.HasFlag(BGFlags::NO_PARITY))
			flagsDecoded |= LTC_NO_PARITY;

		LTC_TV_STANDARD standardDecoded;
		switch(standard) {
		case TVStandard::TV525_60i:
			standardDecoded = LTC_TV_525_60;
			break;
		case TVStandard::TV625_50i:
			standardDecoded = LTC_TV_625_50;
			break;
		case TVStandard::TV1125_60i:
			standardDecoded = LTC_TV_1125_60;
			break;
		case TVStandard::FILM24p:
			standardDecoded = LTC_TV_FILM_24;
			break;
		}

		this->instance = ltc_encoder_create(sampleRate, fps, standardDecoded, flagsDecoded);
	}

	//----------
	Encoder::~Encoder() {
		delete(this->instance);
	}

	//----------
	void Encoder::setTimecode(Timecode ^ timecode) {
		ltc_encoder_set_timecode(this->instance, timecode->getInstance());
	}

	//----------
	Timecode^ Encoder::getTimecode() {
		Timecode^ timecode = gcnew Timecode();
		ltc_encoder_get_timecode(this->instance, timecode->getInstance());
		return timecode;
	}

	//----------
	void Encoder::setFrame(Frame ^ frame) {
		ltc_encoder_set_frame(this->instance, & frame->getInstance()->ltc);
	}

	//----------
	Frame ^ Encoder::getFrame() {
		//Warning : this will be frame without the extended information
		Frame^ frame = gcnew Frame();
		ltc_encoder_get_frame(this->instance, frame->getInstance()->ltc);
		return frame;
	}
}