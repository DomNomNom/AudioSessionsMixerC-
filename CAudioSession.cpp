#include "pch.h"
#include "CAudioSession.h"

CAudioSession::CAudioSession()
{
	pSessionControl = NULL;
	pSessionControl2 = NULL;
	pSessionVolumeCtrl = NULL;
	sid = L"";
}

CAudioSession::~CAudioSession()
{
	/*SAFE_RELEASE(pSessionControl);
	SAFE_RELEASE(pSessionControl2);*/
	int hr;
	CHECK_HR(hr = pSessionControl->UnregisterAudioSessionNotification(eventListener.get()));
	SAFE_DELETE(pSessionControl);
	SAFE_DELETE(pSessionControl2);
	SAFE_DELETE(pSessionVolumeCtrl);
}
