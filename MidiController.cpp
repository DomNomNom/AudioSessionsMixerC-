#include "pch.h"

#include "MidiController.h"

#include <functional>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;


void OnMidiin(double timeStamp, std::vector<unsigned char>* message, void* userData) {
	std::stringstream debugMessage;
	std::copy(message->begin(), message->end(), std::ostream_iterator<int>(debugMessage, " "));
	IMidiControllerEventReceiver* eventReceiver = static_cast<IMidiControllerEventReceiver*> (userData);

	if (message->size() != 3) {
		TRACE("Got a message with weird length: %s\n", debugMessage.str().c_str());
	}
	unsigned char m0 = message->at(0);
	unsigned char m1 = message->at(1);
	unsigned char m2 = message->at(2);
	if (m0 == 144) {  // slider touch down / touch up
		int sliderIndex = m1 - 110;
		if (0 <= sliderIndex && sliderIndex < 8) {
			eventReceiver->OnMidiControllerTouch(sliderIndex, m2 > 0);
		}
		else {
			TRACE("Unhandled MIDI message (might be touch): %s\n", debugMessage.str().c_str());
		}
	}
	else if (m0 == 176) { // slider move
		int sliderIndex = m1 - 70;
		if (0 <= sliderIndex && sliderIndex < 8) {
			eventReceiver->OnMidiControllerDragged(sliderIndex, float(m2) / 127);
		}
		else {
			TRACE("Unhandled MIDI message (might be slider move): %s\n", debugMessage.str().c_str());
		}
	}
	else {
		TRACE("Unhandled MIDI message: %s\n", debugMessage.str().c_str());
	}
}

MidiController::MidiController(IMidiControllerEventReceiver* eventReceiver_) : eventReceiver(eventReceiver_) {
	// Make sure the first calls aren't cached.
	for (int i = 0; i < SLIDER_COUNT; ++i) {
		previousLabels[i] = L"nope...";
		previousPeaks[i] = -1;
	}

	// RtMidiIn constructor
	try {
		midiin = std::make_unique<RtMidiIn>();
	}
	catch (RtMidiError& error) {
		TRACE("midiout error: %s", error.getMessage().c_str());
		return;
	}
	midiin->setCallback(&OnMidiin, eventReceiver);
	// Try find our MidiIn device.
	unsigned int nPorts = midiin->getPortCount();
	TRACE("There are %d MIDI input sources available.\n", nPorts);
	std::string portName;
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			portName = midiin->getPortName(i);
		}
		catch (RtMidiError& error) {
			TRACE("midiin error: %s", error.getMessage().c_str());
			break;
		}
		TRACE("  Input Port #%d: %s\n", i + 1, portName.c_str());
		if (portName == "X-Touch-Ext 0") {
			TRACE("    using this one.\n");
			midiin->openPort(i, portName);
			break;
		}
	}
	TRACE("\n");

	// RtMidiOut constructor
	try {
		midiout = std::make_unique<RtMidiOut>();
	}
	catch (RtMidiError& error) {
		error.printMessage();
		return;
	}
	// Check outputs.
	nPorts = midiout->getPortCount();
	TRACE("There are %d MIDI output ports available.\n", nPorts);
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			portName = midiout->getPortName(i);
		}
		catch (RtMidiError& error) {
			TRACE("midiout error: %s", error.getMessage().c_str());
			break;
		}
		TRACE("  Output Port #%d: %s\n", i + 1, portName.c_str());
		if (portName == "X-Touch-Ext 1") {
			TRACE("    using this one.\n");
			try {
				midiout->openPort(i, portName);
			}
			catch (RtMidiError& error) {
				TRACE("midiout error: %s", error.getMessage().c_str());
				break;
			}
			break;
		}
	}
	TRACE("\n");

	// Set rotary knobs to the middle position. (so the light is hidden behind the knob)
	try {
		for (int i = 0; i < SLIDER_COUNT; ++i) {
			std::vector<unsigned char> message{ 176, (unsigned char)(80 + i), 64 };
			midiout->sendMessage(&message);
		}
	}
	catch (RtMidiError& error) {
		TRACE("midiout error: %s\n", error.getMessage().c_str());
	}

}

MidiController::~MidiController() {}

void MidiController::setSliderPos(int sliderIndex, float volume) {
	//controllerEvent(1, vo.midi_channel, max(0, min(127, int(128 * vo.display)))
	std::vector<unsigned char> message{
		176,
		unsigned char(70 + sliderIndex),
		unsigned char(max(0, min(127, int(128 * volume))))
	};
	try {
		midiout->sendMessage(&message);
	}
	catch (RtMidiError& error) {
		TRACE("midiout error: %s\n", error.getMessage().c_str());
	}
}

const int DISPLAY_WD = 7;  // max number of chars on a line.

void MidiController::sendDisplaySysEx(int sliderIndex, RGB3 color, bool backgroundTop, bool backgroundBot, const CString& txtTop, const CString& txtBot) {
	// TODO: Maybe cache and don't send these sessages.
	std::vector<unsigned char> message{
		0xF0, // start of SysEx
		0x00,
		0x20,
		0x32,
		0x15, // x-touch-ext device ID  which is wrongly documented as 0x42
		0x4c,
		unsigned char(sliderIndex),
		unsigned char(
			(backgroundTop ? 1 << 5 : 0) |
			(backgroundBot ? 1 << 4 : 0) |
			int(color)
		),
	};

	int i;
	for (i = 0; i < DISPLAY_WD && i < txtTop.GetLength(); ++i) {
		message.push_back(unsigned char(txtTop.GetAt(i)));
	}
	for (; i < DISPLAY_WD; ++i) {
		message.push_back(0);
	}

	for (i = 0; i < DISPLAY_WD && i < txtBot.GetLength(); ++i) {
		message.push_back(unsigned char(txtBot.GetAt(i)));
	}
	for (; i < DISPLAY_WD; ++i) {
		message.push_back(0);
	}

	message.push_back(0xf7);  // End of SysEx
	midiout->sendMessage(&message);
}

void MidiController::setLabel(int sliderIndex, const CString& text_) {
	if (previousLabels[sliderIndex] == text_) {
		return;
	}
	previousLabels[sliderIndex] = text_;


	CString text = text_.Left(2 * DISPLAY_WD);
	TRACE("setLabel(%d, %ls)\n", sliderIndex, text);
	if (text != "") {
		RGB3 color = RGB3::white;
		if (text == "explorer") color = RGB3::yellow;
		if (text == "firefox") color = RGB3::yellow;
		if (text == "mumble") color = RGB3::green;
		if (text == "steam") color = RGB3::cyan;
		if (text == "Discord") color = RGB3::cyan;

		if (text == "foobar2000") text = "foobar 2000";
		if (text == "explorer") text = "explore";

		CString txtTop = text.Left(DISPLAY_WD);
		CString txtBot = text.Right(text.GetLength() - DISPLAY_WD);
		sendDisplaySysEx(sliderIndex, color, true, true, txtTop, txtBot);
	}
	else {
		sendDisplaySysEx(sliderIndex, RGB3::black, true, true, L"[OFF]", CString("[OFF]"));
	}
}

void MidiController::setAudioMeter(int sliderIndex, float peak) {
	if (previousPeaks[sliderIndex] == peak) {
		return;
	}
	previousPeaks[sliderIndex] = peak;

	if (peak > 0) {
		//TRACE("setAudioMeter(%d, %f)\n", sliderIndex, peak);
		//peak = sqrt(peak);
		peak = max(peak, .1); // if something is playing, display at least one bar.
	}
	std::vector<unsigned char> message{
		176,
		unsigned char(90 + sliderIndex),
		unsigned char(max(0, min(127, int(128 * peak))))
	};
	midiout->sendMessage(&message);
}