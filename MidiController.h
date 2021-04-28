#pragma once

#include "pch.h"
#include <memory>
#include "RtMidi.h"

class IMidiControllerEventReceiver {
public:
	virtual void OnMidiControllerDragged(int sliderIndex, float volume) = 0;
	virtual void OnMidiControllerTouch(int sliderIndex, bool down) = 0;
};

class MidiController {

	// This class translates between `Slider`-level abstractions and a conected phyical MIDI device.
	// Which currently happens to be a BEHRINGER X-TOUCH EXTENDER.

public:
	MidiController(IMidiControllerEventReceiver* eventReceiver);
	~MidiController();

	void setSliderPos(int sliderIndex, float volume);

private:
	std::unique_ptr<RtMidiIn> midiin;
	std::unique_ptr<RtMidiOut> midiout;

	//void OnMidiin(double timeStamp, std::vector<unsigned char>* message, void* userData);

	// Non-owned. Expected to outlive this object.
	IMidiControllerEventReceiver* eventReceiver;
};