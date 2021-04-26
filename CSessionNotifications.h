
#include<audiopolicy.h>
#include<mmdeviceapi.h>
#include<Audioclient.h>

#include<atlstr.h>

#include "IDomsAudioSessionEvents.h"

// mostly copied from https://docs.microsoft.com/en-us/windows/win32/api/audiopolicy/nn-audiopolicy-iaudiosessionnotification

class CSessionNotifications : public IAudioSessionNotification
{
private:

	LONG             m_cRefAll;
	//HWND m_hwndMain;

	// The app that's expected to outlive this object.
	IDomsAudioSessionEvents* receiver;


public:



	CSessionNotifications(IDomsAudioSessionEvents* receiver_) :
		m_cRefAll(1),
		receiver(receiver_)
	{}
	~CSessionNotifications() {};

	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IAudioSessionNotification) == riid)
		{
			AddRef();
			*ppvInterface = (IAudioSessionNotification*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&m_cRefAll);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&m_cRefAll);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;
	}

	HRESULT OnSessionCreated(IAudioSessionControl* pNewSession)
	{
		return receiver->OnSessionCreated(pNewSession);
	}
};