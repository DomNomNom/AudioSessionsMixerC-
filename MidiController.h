#pragma once

#include "pch.h"
#include <memory>
#include "RtMidi.h"

class IMidiControllerEventReceiver {
public:
	virtual void OnMidiControllerDragged(int sliderIndex, float volume) = 0;
};

class MidiController {
public:
	MidiController(IMidiControllerEventReceiver* eventReceiver);
	~MidiController();

private:
	std::unique_ptr<RtMidiIn> midiin;
	std::unique_ptr<RtMidiOut> midiout;

	// Non-owned. Expected to outlive this object.
	IMidiControllerEventReceiver* eventReceiver;
};