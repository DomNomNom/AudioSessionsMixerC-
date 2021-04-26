
// AudioSessionsMixerC++Dlg.h : header file
//

#pragma once

#include"CAudioSession.h"
#include<vector>
#include "Slider.h"

#include"IDomsAudioSessionEvents.h"
#include"CSessionNotifications.h"

#define SLIDER_COUNT 8

// CAudioSessionsMixerCDlg dialog
class CAudioSessionsMixerCDlg : public CDialogEx, IDomsAudioSessionEvents
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
	DECLARE_MESSAGE_MAP()


private:
	// Things for attaching to AudioSessions.
	IAudioSessionEnumerator* pSessionList;
	IAudioSessionControl* pSessionControl;
	IAudioSessionControl2* pSessionControl2;
	IAudioSessionManager2* pSessionManager;
	CSessionNotifications pSessionNotifications;
	std::vector<CAudioSession> m_AudioSessionList;

	void createSessionManager();
	HRESULT EnumSessions();
	void updateSlidersFromSessions();
	void updateControlsFromSliders();


public:
	// IDomsAudioSessionEvents
	HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(
		CString sid,
		float NewVolume,
		BOOL NewMute,
		LPCGUID EventContext);
	HRESULT STDMETHODCALLTYPE OnStateChanged(
		CString sid,
		AudioSessionState NewState);
	HRESULT STDMETHODCALLTYPE OnSessionDisconnected(
		CString sid,
		AudioSessionDisconnectReason DisconnectReason);
	HRESULT OnSessionCreated(IAudioSessionControl* pNewSession);


	// Things for controling Sliders.
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	Slider sliders[SLIDER_COUNT];
	CStatic textControls[SLIDER_COUNT];
	CSliderCtrl sliderControls[SLIDER_COUNT];
	void OnVolumeIntent(const Slider& slider);
	void SwapSliderToPreferredIndex(CString label, int index);
};
