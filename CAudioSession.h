#pragma once

#include<audiopolicy.h>
#include<mmdeviceapi.h>
#include<Audioclient.h>

#include<atlstr.h>

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;

#define CHECK_HR(x) if (FAILED(x)) { TRACE("Exception Throw\n"); }


class CAudioSession
{
public:
	CAudioSession();
	~CAudioSession();

public:
	CString exeName;
	CString exeFileName;

	IAudioSessionControl* pSessionControl;
	IAudioSessionControl2* pSessionControl2;

	ISimpleAudioVolume* pSessionVolumeCtrl;

	UINT volume;
	// TODO: time updated
};

