using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NAudio.CoreAudioApi;
using VVVV.PluginInterfaces.V2;

namespace VVVV.Nodes.NAudio
{
	#region PluginInfo
	[PluginInfo(Name = "ListDevices", Category = "NAudio", Help = "Enumerate WASAPI devices", Tags = "", Author = "elliotwoods", AutoEvaluate = true)]
	#endregion PluginInfo
	public class ListDevicesNode : IPluginEvaluate
	{
		[Input("Refresh", IsBang = true, IsSingle = true)]
		ISpread<bool> FInRefresh;

		[Input("Type", IsSingle=true)]
		IDiffSpread<DataFlow> FInDeviceType;

		[Input("State", IsSingle = true)]
		IDiffSpread<DeviceState> FInDeviceState;

		[Output("Device")]
		ISpread<MMDevice> FOutDevices;

		[Output("Name")]
		ISpread<string> FOutName;

		[Output("State")]
		ISpread<string> FOutState;

		bool firstRun = true;

		public void Evaluate(int SpreadMax)
		{
			if (firstRun || FInRefresh[0] || FInDeviceType.IsChanged)
			{
				firstRun = false;

				var enumerator = new MMDeviceEnumerator();
				var devices = enumerator.EnumerateAudioEndPoints(FInDeviceType[0], FInDeviceState[0]).ToArray();

				FOutDevices.SliceCount = devices.Count();
				FOutName.SliceCount = devices.Count();
				FOutState.SliceCount = devices.Count();

				int index = 0;

				foreach (var device in devices)
				{
					FOutDevices[index] = device;
					FOutName[index] = device.FriendlyName;
					FOutState[index] = device.State.ToString();
					index++;
				}
			}
		}
	}
}
