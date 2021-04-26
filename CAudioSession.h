#pragma once

#include<audiopolicy.h>
#include<mmdeviceapi.h>
#include<Audioclient.h>

#include<atlstr.h>
#include "CAudioSessionEvents.h"

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define CHECK_HR(x) if (FAILED(x)) { TRACE("Exception Throw\n"); }


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
	CAudioSessionEvents* eventListener;
};

