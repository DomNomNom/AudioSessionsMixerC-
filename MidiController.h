#pragma once

#include "pch.h"
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
	// Owned. TODO: Unique_ptr
	RtMidiIn* midiin;
	RtMidiOut* midiout;

	// Expected to outlive this object
	IMidiControllerEventReceiver* eventReceiver;
};