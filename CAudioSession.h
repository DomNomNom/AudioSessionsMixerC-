#pragma once

#include <memory>
#include<audiopolicy.h>
#include<mmdeviceapi.h>
#include<Audioclient.h>
#include<endpointvolume.h>

#include<atlstr.h>
#include "CAudioSessionEvents.h"

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define CHECK_HR(x) hr = (x); if (hr != S_OK) { TRACE("Exception Throw: %d\n", hr); }


class CAudioSession
{
public:
	CAudioSession();
	~CAudioSession();

public:

	LPWSTR sid;  // Session identifier from pSessionControl2

	// These are all owning pointers.
	IAudioSessionControl* pSessionControl;
	IAudioSessionControl2* pSessionControl2;
	ISimpleAudioVolume* pSessionVolumeCtrl;

	std::unique_ptr<CAudioSessionEvents> eventListener;
};

