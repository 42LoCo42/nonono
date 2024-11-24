package main

import (
	"time"

	"github.com/gordonklaus/portaudio"
)

func main() {
	if err := portaudio.Initialize(); err != nil {
		panic(err)
	}
	defer portaudio.Terminate()

	devices, err := portaudio.Devices()
	if err != nil {
		panic(err)
	}
	var chosenDevice *portaudio.DeviceInfo
	for _, device := range devices {
		if device.Name == "pulse" {
			chosenDevice = device
			break
		}
	}
	if chosenDevice == nil {
		panic("no pulse device found")
	}

	params := portaudio.LowLatencyParameters(chosenDevice, chosenDevice)
	params.Input.Channels = 2
	params.Output.Channels = 2
	stream, err := portaudio.OpenStream(params, callback)
	if err != nil {
		panic(err)
	}
	if err := stream.Start(); err != nil {
		panic(err)
	}
	defer stream.Stop()

	select {}
}

func callback(
	in []float32,
	out []float32,
) {
	copy(out, in)
	time.Sleep(time.Millisecond * 2)
}
