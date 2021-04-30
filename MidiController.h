#pragma once

#include "pch.h"
#include <memory>
#include "RtMidi.h"

#define SLIDER_COUNT 8

class IMidiControllerEventReceiver {
public:
	virtual void OnMidiControllerDragged(int sliderIndex, float volume) = 0;
	virtual void OnMidiControllerTouch(int sliderIndex, bool down) = 0;
};

enum RGB3 {
	black = 0,
	red = 1,
	green = 2,
	yellow = 3,
	blue = 4,
	magenta = 5,
	cyan = 6,
	white = 7,
};

class MidiController {

	// This class translates between `Slider`-level abstractions and a conected phyical MIDI device.
	// Which currently happens to be a BEHRINGER X-TOUCH EXTENDER.

public:
	MidiController(IMidiControllerEventReceiver* eventReceiver);
	~MidiController();

	void setSliderPos(int sliderIndex, float volume);
	void setLabel(int sliderIndex, const CString& txt);
	void setAudioMeter(int sliderIndex, float peak);

private:
	std::unique_ptr<RtMidiIn> midiin;
	std::unique_ptr<RtMidiOut> midiout;

	CString previousLabels[SLIDER_COUNT];
	void sendDisplaySysEx(int sliderIndex, RGB3 color, bool backgroundTop, bool backgroundBot, const CString& txtTop, const CString& txtBot);

	float previousPeaks[SLIDER_COUNT];

	// Non-owned. Expected to outlive this object.
	IMidiControllerEventReceiver* eventReceiver;
};