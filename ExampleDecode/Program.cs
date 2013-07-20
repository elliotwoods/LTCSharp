using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using NAudio.Wave;

namespace ExampleDecode
{
	class Program
	{
		static LTCSharp.Decoder FDecoder;

		[STAThread]
		static void Main(string[] args)
		{
			WaveInExample();
			//FileLoadExample();
		}

		static void waveIn_DataAvailable(object sender, WaveInEventArgs e)
		{
			lock (FDecoder)
			{
				FDecoder.WriteAsU16(e.Buffer, e.BytesRecorded, 0);
			}
		}

		static void WaveInExample()
		{
			FDecoder = new LTCSharp.Decoder(48000, 25, 32);

			var waveIn = new WaveInEvent();
			Console.WriteLine("Device format: " + waveIn.WaveFormat.ToString());

			FDecoder = new LTCSharp.Decoder(waveIn.WaveFormat.SampleRate, 25, 32);
			waveIn.DataAvailable += waveIn_DataAvailable;
			waveIn.StartRecording();

			Stopwatch timer = new Stopwatch();
			timer.Start();
			while (timer.Elapsed < new TimeSpan(0, 0, 20))
			{
				lock (FDecoder)
				{
					if (FDecoder.GetQueueLength() > 0)
					{
						try
						{
							var frame = FDecoder.Read();
							var timecode = frame.getTimecode();
							Console.WriteLine(timecode.ToString());
						}
						catch (Exception e)
						{
							Console.Write(e);
						}
					}
					else
					{
						Thread.Sleep(10);
					}
				}
			}
			timer.Stop();
		}

		static void FileLoadExample()
		{
			OpenFileDialog selectFileDialog = new OpenFileDialog();
			selectFileDialog.Filter = "wav files (*.wav)|*.wav";
			selectFileDialog.RestoreDirectory = true;
			selectFileDialog.ShowDialog();
			var wavePlayer = new WaveFileReader(selectFileDialog.FileName);
			Console.WriteLine("File format: " + wavePlayer.WaveFormat.ToString());

			FDecoder = new LTCSharp.Decoder(wavePlayer.WaveFormat.SampleRate, 25, 32);

			int size = 1600;
			byte[] buffer = new byte[size];
			int total = 0;
			while (wavePlayer.Position < wavePlayer.Length)
			{
				var task = wavePlayer.Read(buffer, 0, size);

				FDecoder.WriteAsU16(buffer, size / 2, total);

				total += size / 2;

				try
				{
					var frame = FDecoder.Read();
					var timecode = frame.getTimecode();
					Console.WriteLine(wavePlayer.CurrentTime.ToString() + "\t" + timecode.ToString());
				}
				catch
				{
					//no frames available
				}
			}

			Console.WriteLine("END OF FILE");
		}
	}
}
