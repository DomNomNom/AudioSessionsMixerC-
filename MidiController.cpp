#include "pch.h"

#include "MidiController.h"

#include <functional>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define SLIDER_COUNT 8

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
}

MidiController::~MidiController() {}

void MidiController::setSliderPos(int sliderIndex, float volume) {
	//controllerEvent(1, vo.midi_channel, max(0, min(127, int(128 * vo.display)))
	std::vector<unsigned char> message;
	message.push_back(176);
	message.push_back(70 + sliderIndex);
	message.push_back(max(0, min(127, (unsigned char)(128 * volume))));
	try {
		midiout->sendMessage(&message);
	}
	catch (RtMidiError& error) {
		TRACE("midiout error: %s\n", error.getMessage().c_str());
	}
}