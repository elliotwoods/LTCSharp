using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using NAudio.Wave;

namespace ExampleEncode
{
	class Source : IWaveProvider
	{
		LTCSharp.Encoder FEncoder;

		public Source()
		{
			FEncoder = new LTCSharp.Encoder(48000, 30, LTCSharp.TVStandard.TV525_60i, LTCSharp.BGFlags.NONE);
		}

		public int Read(byte[] buffer, int offset, int count)
		{
			lock (FEncoder)
			{
				Console.Write("bip");
				FEncoder.encodeFrame();
				int size = FEncoder.getBuffer(buffer, offset);
				Console.WriteLine(size);
				return size;
			}
		}

		public LTCSharp.Encoder Encoder
		{
			get
			{
				return this.FEncoder;
			}
		}

		public WaveFormat WaveFormat
		{
			get
			{
				return new WaveFormat(48000, 1);
			}
		}

		public void SetTimecode(LTCSharp.Timecode timecode)
		{
			lock (FEncoder)
			{
				FEncoder.setTimecode(timecode);
			}
		}
	}

	class Program
	{
		static void Main(string[] args)
		{
			var waveOut = new WaveOut();
			var encoder = new Source();
			waveOut.Init(encoder);
			waveOut.Play();

			Stopwatch timer = new Stopwatch();
			timer.Start();
			while (timer.Elapsed < new TimeSpan(0, 0, 5))
			{
				encoder.SetTimecode(new LTCSharp.Timecode(
					timer.Elapsed.Hours,
					timer.Elapsed.Minutes,
					timer.Elapsed.Seconds,
					(int) ((float)timer.Elapsed.Milliseconds / 1000.0f * 30.0f)));
				Thread.Sleep(10);
			}
			timer.Stop();

			waveOut.Stop();
			waveOut.Dispose();
		}
	}
}
