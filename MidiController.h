#pragma once

#include "RtMidi.h"

class MidiControllerEventReceiver {
public:
	virtual void OnMidiControllerDragged(int sliderIndex, float volume) = 0;
};

class MidiController {
public:
	MidiController();
	~MidiController();

private:
	RtMidiIn* midiin;
	RtMidiOut* midiout;
};