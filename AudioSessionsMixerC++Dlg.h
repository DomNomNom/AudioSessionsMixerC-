
// AudioSessionsMixerC++Dlg.h : header file
//

#pragma once

#include"CAudioSession.h"
#include<vector>
#include<memory>
#include "Slider.h"

#include"IDomsAudioSessionEvents.h"
#include"CSessionNotifications.h"
#include "MidiController.h"

#define SLIDER_COUNT 8

// CAudioSessionsMixerCDlg dialog
class CAudioSessionsMixerCDlg : public CDialogEx, IDomsAudioSessionEvents, IMidiControllerEventReceiver
{
	// Construction
public:
	CAudioSessionsMixerCDlg(CWnd* pParent = nullptr);	// standard constructor
	~CAudioSessionsMixerCDlg();
	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUDIOSESSIONSMIXERC_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Dialog related things
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIdEvent);
	DECLARE_MESSAGE_MAP()



private:
	// Things for attaching to AudioSessions.
	IAudioSessionEnumerator* pSessionList;
	IAudioSessionControl* pSessionControl;
	IAudioSessionControl2* pSessionControl2;
	IAudioSessionManager2* pSessionManager;
	CSessionNotifications pSessionNotifications;
	std::vector<std::unique_ptr<CAudioSession>> m_AudioSessionList;

	void createSessionManager();

	void updateEverythingFromOS() {
		updateSessionsFromManager();
		updateSlidersFromSessions();
		updateControlsFromSliders();
		TRACE("updateEverythingFromOS finished with %d sessions.\n", m_AudioSessionList.size());
	}
	void updateSessionsFromManager();
	void updateSlidersFromSessions();
	void updateControlsFromSliders();



	// Things for controling Sliders.
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	Slider sliders[SLIDER_COUNT];
	CStatic textControls[SLIDER_COUNT];
	CSliderCtrl sliderControls[SLIDER_COUNT];
	void OnVolumeIntent(const Slider& slider);
	void SwapSliderToPreferredIndex(CString label, int index);


public:
	// These methods implement IDomsAudioSessionEvents.
	HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		const LPWSTR& sid,
		float NewVolume,
		BOOL NewMute,
		LPCGUID EventContext);
	HRESULT STDMETHODCALLTYPE OnStateChanged(
		const LPWSTR& sid,
		AudioSessionState NewState);
	HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		const LPWSTR& sid,
		AudioSessionDisconnectReason DisconnectReason);
	HRESULT OnSessionCreated(IAudioSessionControl* pNewSession);


private:
	// Things for finding stuff by session id's.
	// We cache the last successful find to save some string matching time.
	// Returns a negative number if not found.
	int findSessionIndexBySid(const LPWSTR& sid);
	int findSliderIndexBySid(const LPWSTR& sid);
	int lastFoundSessionIndex;
	int lastFoundSliderIndex;


	// MIDI device
private:
	MidiController midiController;
public:  // Implement MidiControllerEventReceiver
	void OnMidiControllerDragged(int sliderIndex, float volume);
	void CAudioSessionsMixerCDlg::OnMidiControllerTouch(int sliderIndex, bool down);

};
