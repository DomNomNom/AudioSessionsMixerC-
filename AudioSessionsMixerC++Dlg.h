
// AudioSessionsMixerC++Dlg.h : header file
//

#pragma once

#include"CAudioSession.h"
#include<vector>
#include "Slider.h"

#define SLIDER_COUNT 8

// CAudioSessionsMixerCDlg dialog
class CAudioSessionsMixerCDlg : public CDialogEx
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

	void createSessionManager();
	HRESULT EnumSessions();
	void initCmbWithAudSessionName();
	void changeSelectedAudioSessionVol(UINT vol);

	std::vector<CAudioSession> m_AudioSessionList;

	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult);
	CComboBox m_CmbAudioSession;
	afx_msg void OnCbnSelchangeComboAudsession();
	CSliderCtrl m_SldrAudSessionVol;
	afx_msg void OnTRBNThumbPosChangingSliderAudsessionVol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	CSliderCtrl sliderControls[SLIDER_COUNT];
	CStatic textControls[SLIDER_COUNT];
	Slider sliders[SLIDER_COUNT];
};
