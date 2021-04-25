
// AudioSessionsMixerC++Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "AudioSessionsMixerC++.h"
#include "AudioSessionsMixerC++Dlg.h"
#include "afxdialogex.h"
#include <string>
#include <TlHelp32.h>
#include <utility>
#include <WinUser.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SLIDER_CONTROL_BASE_ID 2000
#define STATIC_CONTROL_BASE_ID 3000



//////////////////////////////////////
//HWND GetWindowHandleFromProcessID(DWORD dwProcessID)
//{
//	// find all hWnds (vhWnds) associated with a process id (dwProcessID)
//	HWND hCurWnd = NULL;
//	HWND h = ::GetTopWindow(0);
//	while (h)
//	{
//		DWORD pid;
//		DWORD dwTheardId = ::GetWindowThreadProcessId(h, &pid);
//
//
//
//		if (pid == dwProcessID)
//		{
//			// here h is the handle to the window
//			return h;
//			break;
//		}
//		h = ::GetNextWindow(h, GW_HWNDNEXT);
//	}
//	return NULL;
////	do
////	{
////		hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);
////		DWORD dwProcessID = 0;
////		GetWindowThreadProcessId(hCurWnd, &dwProcessID);
////		if (dwProcessID == dwProcessID)
////		{
//////			vhWnds.push_back(hCurWnd);  // add the found hCurWnd to the vector
////	//		wprintf(L"Found hWnd %d\n", hCurWnd);
////			return hCurWnd;
////		}
////	} while (hCurWnd != NULL);
////	return NULL;
//
//
//}
//////////
std::wstring GetProcName(DWORD aPid)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(processInfo);
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
	{
		//	std::wcout << "can't get a process snapshot ";
		return std::wstring();
	}

	for (BOOL bok = Process32First(processesSnapshot, &processInfo); bok; bok = Process32Next(processesSnapshot, &processInfo))
	{
		if (aPid == processInfo.th32ProcessID)
		{
			//	std::wcout << "found running process: " << processInfo.szExeFile;
			CloseHandle(processesSnapshot);
			return processInfo.szExeFile;
		}

	}
	//	std::wcout << "no process with given pid" << aPid;
	CloseHandle(processesSnapshot);
	return std::wstring();
}
std::string ProcessIdToName(DWORD processId)
{
	std::string ret;
	HANDLE handle = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION,
		FALSE,
		processId /* This is the PID, you can find one from windows task manager */
	);
	if (handle)
	{
		DWORD buffSize = 1024;
		CHAR buffer[1024];
		if (QueryFullProcessImageNameA(handle, 0, buffer, &buffSize))
		{
			ret = buffer;
		}
		else
		{
			printf("Error GetModuleBaseNameA : %lu", GetLastError());
		}
		CloseHandle(handle);
	}
	else
	{
		printf("Error OpenProcess : %lu", GetLastError());
	}
	return ret;
}



// CAboutDlg dialog used for App About



class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAudioSessionsMixerCDlg dialog



CAudioSessionsMixerCDlg::CAudioSessionsMixerCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AUDIOSESSIONSMIXERC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pswSession = NULL;

	pSessionList = NULL;
	pSessionControl = NULL;
	pSessionControl2 = NULL;
	pSessionManager = NULL;
}

void CAudioSessionsMixerCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_AUDSESSION, m_CmbAudioSession);
	DDX_Control(pDX, IDC_SLIDER_AUDSESSION_VOL, m_SldrAudSessionVol);
	int i = 0;
	for (CSliderCtrl& slider : sliderControls) {
		DDX_Control(pDX, SLIDER_CONTROL_BASE_ID + i, slider);
		i += 1;
	}
}

BEGIN_MESSAGE_MAP(CAudioSessionsMixerCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_AUDSESSION, &CAudioSessionsMixerCDlg::OnCbnSelchangeComboAudsession)
#pragma warning(suppress : C26454) ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_AUDSESSION_VOL, &CAudioSessionsMixerCDlg::OnNMCustomdrawSlider1)
#pragma warning(suppress : C26454) ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_AUDSESSION_VOL, &CAudioSessionsMixerCDlg::OnTRBNThumbPosChangingSliderAudsessionVol)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CAudioSessionsMixerCDlg message handlers


BOOL CAudioSessionsMixerCDlg::OnInitDialog()
{
	{
		const int WD = 100;  // Width of each slider column
		const int TEXT_BOTTOM = 50;
		int i = 0;
		for (CSliderCtrl& slider : sliderControls) {
			slider.Create(WS_CHILD | WS_VISIBLE | TBS_VERT, CRect(i * WD, TEXT_BOTTOM, (i + 1) * WD, 280), this, i + SLIDER_CONTROL_BASE_ID);
			slider.SetPos(0);
			//slider.orientation/
			//slider.SetTicFreq(100);
			i += 1;
		}
		i = 0;
		for (CStatic& textControl : textControls) {
			CString txt;
			txt.Format(L"HI %d", i);
			textControl.Create(txt, WS_CHILD | WS_VISIBLE, CRect(i * WD, 10, (i + 1) * WD, TEXT_BOTTOM), this, STATIC_CONTROL_BASE_ID);
			//std::string txt = "hi ";
			txt.Format(L"Ho2\n %d", i);
			textControl.SetWindowTextW(txt);
			i += 1;
		}
	}
	CDialogEx::OnInitDialog();

	//DWORD style = GetWindowLong(m_SldrAudSessionVol, GWL_STYLE);
	//for (CSliderCtrl& slider : sliderControls) {
	//	SetWindowLong(slider, GWL_STYLE, style);
	//}
	SetWindowLong(m_SldrAudSessionVol, GWL_STYLE, 0); // hide
	SetWindowLong(m_CmbAudioSession, GWL_STYLE, 0); // hide


	int hr;
	CHECK_HR(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED));

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	// TODO: Add extra initialization here
	m_SldrAudSessionVol.SetRange(-100, 0, TRUE);
	m_SldrAudSessionVol.SetTicFreq(1);
	m_SldrAudSessionVol.SetPos(0);


	createSessionManager();

	EnumSessions();

	if (m_AudioSessionList.size() > 0)
		initCmbWithAudSessionName();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAudioSessionsMixerCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAudioSessionsMixerCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAudioSessionsMixerCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CAudioSessionsMixerCDlg::EnumSessions()
{
	if (!pSessionManager)
	{
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	int cbSessionCount = 0;


	// Get the current list of sessions.
	CHECK_HR(hr = pSessionManager->GetSessionEnumerator(&pSessionList));

	// Get the session count.
	CHECK_HR(hr = pSessionList->GetCount(&cbSessionCount));

	for (int index = 0; index < cbSessionCount; index++)
	{
		CoTaskMemFree(pswSession);
		CAudioSession* sessionObj = new CAudioSession();

		SAFE_RELEASE(sessionObj->pSessionControl);

		// Get the <n>th session.
		CHECK_HR(hr = pSessionList->GetSession(index, &sessionObj->pSessionControl));

		//		pSessionControl->AddRef();
				  // Get the extended session control interface pointer.
		CHECK_HR(hr = sessionObj->pSessionControl->QueryInterface(
			__uuidof(IAudioSessionControl2), (void**)&sessionObj->pSessionControl2));

		//get session volume control
		CHECK_HR(hr = sessionObj->pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
			(void**)&sessionObj->pSessionVolumeCtrl));

		float volf = 0.0;
		sessionObj->pSessionVolumeCtrl->GetMasterVolume(&volf);
		sessionObj->volume = int(100 * volf);
		//		pSessionManager->GetSimpleAudioVolume(, false, &sessionObj->pSessionVolumeCtrl);


				//apps session name is empty volume mixer shows process name 
				//CHECK_HR(hr = pSessionControl->GetDisplayName(&pswSession));
				//wprintf_s(L"Session Name: %s\n", pswSession);

				//so getting process id and then its name for reference of its session
		DWORD id = NULL;
		CHECK_HR(hr = sessionObj->pSessionControl2->GetProcessId(&id));//audio session owner process id  

//		TRACE("%d\n",id);

		//for full path of app
		sessionObj->exeFileName = CString(ProcessIdToName(id).c_str());

		//app exe name only
		sessionObj->exeName = CString(GetProcName(id).c_str());

		m_AudioSessionList.push_back(*sessionObj);


		//CString str = L"";
		//HWND hwndo = NULL;//;
		//int i=GetWindowTextA(GetWindowHandleFromProcessID(id), LPSTR(str.GetString()), NULL);


	}


	return hr;

}

void CAudioSessionsMixerCDlg::initCmbWithAudSessionName()
{
	for (int i = 0; i < m_AudioSessionList.size(); i++)
		m_CmbAudioSession.AddString(m_AudioSessionList[i].exeName);
}

void CAudioSessionsMixerCDlg::changeSelectedAudioSessionVol(UINT vol)
{
	int sel = m_CmbAudioSession.GetCurSel();
	if (sel < 0) return;
	float value = float(vol) / 100.0f;
	m_AudioSessionList[sel].pSessionVolumeCtrl->SetMasterVolume(value, NULL);
}

void CAudioSessionsMixerCDlg::createSessionManager()
{
	HRESULT hr = S_OK;

	IMMDevice* pDevice = NULL;
	IMMDeviceEnumerator* pEnumerator = NULL;

	// Create the device enumerator.
	CHECK_HR(hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&pEnumerator));

	// Get the default audio device.
	CHECK_HR(hr = pEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &pDevice));

	// Get the session manager.
	CHECK_HR(hr = pDevice->Activate(
		__uuidof(IAudioSessionManager2), CLSCTX_ALL,
		NULL, (void**)&pSessionManager));

}

CAudioSessionsMixerCDlg::~CAudioSessionsMixerCDlg()
{
	CoTaskMemFree(pswSession);
	SAFE_RELEASE(pSessionList);
}



void CAudioSessionsMixerCDlg::OnNMCustomdrawSlider1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here


	*pResult = 0;
}


void CAudioSessionsMixerCDlg::OnCbnSelchangeComboAudsession()
{
	int sel = m_CmbAudioSession.GetCurSel();
	m_SldrAudSessionVol.ClearSel();
	if (sel < 0) return;
	m_SldrAudSessionVol.SetPos(-1 * m_AudioSessionList[sel].volume);
}


void CAudioSessionsMixerCDlg::OnTRBNThumbPosChangingSliderAudsessionVol(NMHDR* pNMHDR, LRESULT* pResult)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: Add your control notification handler code here


	*pResult = 0;
}


void CAudioSessionsMixerCDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);

	// Check which slider sent the notification  
	if (pSlider == &m_SldrAudSessionVol)
	{
		int vol = -1 * m_SldrAudSessionVol.GetPos();
		changeSelectedAudioSessionVol(vol);
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

