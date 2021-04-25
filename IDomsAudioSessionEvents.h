#pragma once

class IDomsAudioSessionEvents {
	// A Interface such that AudioSessionMixer dialog can receive events
	// from specific sessions and know which sessions they come from.

public:
	virtual ~IDomsAudioSessionEvents() {}

	virtual HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		CSTRING sid,
		float NewVolume,
		BOOL NewMute,
		LPCGUID EventContext) = 0;

	virtual HRESULT STDMETHODCALLTYPE OnStateChanged(
		CSTRING sid,
		AudioSessionState NewState) = 0;

	virtual HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		CSTRING sid,
		AudioSessionDisconnectReason DisconnectReason) = 0;

};