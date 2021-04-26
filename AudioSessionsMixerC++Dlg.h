
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
public:

	LPWSTR pswSession;

	IAudioSessionEnumerator* pSessionList;
	IAudioSessionControl* pSessionControl;
	IAudioSessionControl2* pSessionControl2;
	IAudioSessionManager2* pSessionManager;
	CSessionNotifications pSessionNotifications;

	void createSessionManager();
	HRESULT EnumSessions();
	void initCmbWithAudSessionName();
	void changeSelectedAudioSessionVol(UINT vol);

	void updateSlidersFromSessions();
	void updateControlsFromSliders();

	std::vector<CAudioSession> m_AudioSessionList;


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


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void SwapSliderToPreferredIndex(CString label, int index);


public:
	afx_msg void OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCbnSelchangeComboAudsession();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTRBNThumbPosChangingSliderAudsessionVol(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnSliderControlChange(NMHDR* pNMHDR, LRESULT* pResult);
	void OnVolumeIntent(const Slider& slider);

	CComboBox m_CmbAudioSession;
	CSliderCtrl m_SldrAudSessionVol;

	CSliderCtrl sliderControls[SLIDER_COUNT];
	CStatic textControls[SLIDER_COUNT];
	Slider sliders[SLIDER_COUNT];
};
