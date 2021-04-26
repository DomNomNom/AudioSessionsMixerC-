#pragma once
#include<atlstr.h>

class IDomsAudioSessionEvents {
	// A Interface such that AudioSessionMixer dialog can receive events
	// from specific sessions and know which sessions they come from.

public:
	virtual ~IDomsAudioSessionEvents() {}

	virtual HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		CString sid,
		float NewVolume,
		BOOL NewMute,
		LPCGUID EventContext) = 0;

	virtual HRESULT STDMETHODCALLTYPE OnStateChanged(
		CString sid,
		AudioSessionState NewState) = 0;

	virtual HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		CString sid,
		AudioSessionDisconnectReason DisconnectReason) = 0;

	// from IAudioSessionNotification
	virtual HRESULT OnSessionCreated(IAudioSessionControl* pNewSession) = 0;
};