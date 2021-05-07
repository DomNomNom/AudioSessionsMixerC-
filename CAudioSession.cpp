#include "pch.h"
#include "CAudioSession.h"

CAudioSession::CAudioSession()
{
	sid = L"";
	state = AudioSessionState::AudioSessionStateInactive;
	pSessionControl = NULL;
	pSessionControl2 = NULL;
	pSessionVolumeCtrl = NULL;
	pAudioMeterInformation = NULL;
}

CAudioSession::~CAudioSession()
{
	int hr;
	CHECK_HR(hr = pSessionControl->UnregisterAudioSessionNotification(eventListener.get()));
	/*SAFE_RELEASE(pSessionControl);
	SAFE_RELEASE(pSessionControl2);*/
	/*
	SAFE_DELETE(pSessionControl);
	SAFE_DELETE(pSessionControl2);
	SAFE_DELETE(pSessionVolumeCtrl);
	*/
}
