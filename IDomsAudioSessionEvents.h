#pragma once
#include<atlstr.h>

class IDomsAudioSessionEvents {
	// A Interface such that AudioSessionMixer dialog can receive events
	// from specific sessions and know which sessions they come from.

public:
	virtual ~IDomsAudioSessionEvents() {}

	virtual HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		const LPWSTR& sid,
		float NewVolume,
		BOOL NewMute,
		LPCGUID EventContext) = 0;

	virtual HRESULT STDMETHODCALLTYPE OnStateChanged(
		const LPWSTR& sid,
		AudioSessionState NewState) = 0;

	virtual HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		const LPWSTR& sid,
		AudioSessionDisconnectReason DisconnectReason) = 0;

	// from IAudioSessionNotification
	virtual HRESULT OnSessionCreated(IAudioSessionControl* pNewSession) = 0;
};