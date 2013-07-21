using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NAudio.CoreAudioApi;
using NAudio.Wave;
using VVVV.PluginInterfaces.V2;

namespace VVVV.Nodes.LTC
{
	#region PluginInfo
	[PluginInfo(Name = "Decoder", Category = "Timecode", Version = "LTC", Help = "Decode audio as LTC timecode", Tags = "", Author = "elliotwoods", AutoEvaluate = true)]
	#endregion PluginInfo
	public class DecoderNode : IPluginEvaluate
	{
		class DecodeInstance : IDisposable
		{
			LTCSharp.Decoder FDecoder;
			WasapiCapture FCapture;
			int FChannel;

			public DecodeInstance(MMDevice device, uint channels, uint channel, double framerate)
			{
				if (device == null)
					throw (new Exception("No device selected"));

				FChannel = (int) channel;
				FCapture = new WasapiCapture(device);
				FCapture.WaveFormat = new WaveFormat(44100, 8, (int) channels);
				channels = (uint) FCapture.WaveFormat.Channels;

				if (channel >= channels)
				{
					throw (new Exception("Capture channel index out of range"));
				}

				FDecoder = new LTCSharp.Decoder(FCapture.WaveFormat.SampleRate, (int) framerate, 32);

				FCapture.DataAvailable += FCapture_DataAvailable;
				FCapture.StartRecording();
			}

			unsafe void FCapture_DataAvailable(object sender, WaveInEventArgs e)
			{
				lock (FDecoder)
				{
					byte[] downSampled = new byte[e.BytesRecorded / 2];
					for (int i = 0; i < e.BytesRecorded / 2; i++)
					{
						downSampled[i] = e.Buffer[i * 2 + FChannel];
					}

					FDecoder.Write(downSampled, e.BytesRecorded / 2, 0);
				}
			}

			public LTCSharp.Timecode Timecode
			{
				get
				{
					return FDecoder.Read().getTimecode();
				}
			}

			public void Dispose()
			{
				FCapture.StopRecording();
			}
		}

		[Input("Device")]
		IDiffSpread<MMDevice> FInDevice;

		[Input("Framerate", DefaultValue=25)]
		IDiffSpread<int> FInFramerate;

		[Input("Channel Count", DefaultValue=2)]
		IDiffSpread<uint> FInChannels;

		[Input("Channel Index")]
		IDiffSpread<uint> FInChannel;

		[Output("Timecode")]
		ISpread<LTCSharp.Timecode> FOutTimecode;

		[Output("Status")]
		ISpread<string> FOutStatus;

		Spread<DecodeInstance> FInstances = new Spread<DecodeInstance>(0);

		public void Evaluate(int SpreadMax)
		{
			if (FInDevice.IsChanged || FInFramerate.IsChanged || FInChannel.IsChanged || FInChannels.IsChanged)
			{
				foreach (var instance in FInstances)
				{
					if (instance != null)
					{
						instance.Dispose();
					}
				}

				FInstances.SliceCount = 0;
				FOutStatus.SliceCount = SpreadMax;
				FOutTimecode.SliceCount = SpreadMax;

				for (int i = 0; i < SpreadMax; i++)
				{
					try
					{
						var instance = new DecodeInstance(FInDevice[i], FInChannels[i], FInChannel[i], FInFramerate[i]);
						FInstances.Add(instance);
						FOutStatus[i] = "OK";
					}
					catch (Exception e)
					{
						FInstances.Add(null);
						FOutStatus[i] = e.Message;
					}
				}
			}

			for (int i = 0; i < FInstances.SliceCount; i++)
			{
				if (FInstances[i] != null)
				{
					try
					{
						FOutTimecode[i] = FInstances[i].Timecode;
					}
					catch
					{
					}
				}
				else
				{
					FOutTimecode[i] = null;
				}
			}
		}
	}
}
