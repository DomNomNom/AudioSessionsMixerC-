
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
#define DEAD_SESSION_TIMER_ID 1050
#define AUDIO_METER_TIMER_ID 1051



//////////////////////////////////////
//HWND GetWindowHandleFromProcessID(DWORD dwProcessID)
//{
//  // find all hWnds (vhWnds) associated with a process id (dwProcessID)
//  HWND hCurWnd = NULL;
//  HWND h = ::GetTopWindow(0);
//  while (h)
//  {
//      DWORD pid;
//      DWORD dwTheardId = ::GetWindowThreadProcessId(h, &pid);
//
//
//
//      if (pid == dwProcessID)
//      {
//          // here h is the handle to the window
//          return h;
//          break;
//      }
//      h = ::GetNextWindow(h, GW_HWNDNEXT);
//  }
//  return NULL;
////    do
////    {
////        hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);
////        DWORD dwProcessID = 0;
////        GetWindowThreadProcessId(hCurWnd, &dwProcessID);
////        if (dwProcessID == dwProcessID)
////        {
//////          vhWnds.push_back(hCurWnd);  // add the found hCurWnd to the vector
////    //      wprintf(L"Found hWnd %d\n", hCurWnd);
////            return hCurWnd;
////        }
////    } while (hCurWnd != NULL);
////    return NULL;
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
		//  std::wcout << "can't get a process snapshot ";
		return std::wstring();
	}

	for (BOOL bok = Process32First(processesSnapshot, &processInfo); bok; bok = Process32Next(processesSnapshot, &processInfo))
	{
		if (aPid == processInfo.th32ProcessID)
		{
			//  std::wcout << "found running process: " << processInfo.szExeFile;
			CloseHandle(processesSnapshot);
			return processInfo.szExeFile;
		}

	}
	//  std::wcout << "no process with given pid" << aPid;
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
	: CDialogEx(IDD_AUDIOSESSIONSMIXERC_DIALOG, pParent),
	pSessionNotifications(static_cast<IDomsAudioSessionEvents*> (this)),
	midiController(static_cast<IMidiControllerEventReceiver*>(this)),
	lastFoundSessionIndex(0),
	lastFoundSliderIndex(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	pSessionList = NULL;
	pSessionControl = NULL;
	pSessionControl2 = NULL;
	pSessionManager = NULL;
	for (Slider& slider : sliders) {
		slider.connected = false;
	}
}

void CAudioSessionsMixerCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CAudioSessionsMixerCDlg message handlers


BOOL CAudioSessionsMixerCDlg::OnInitDialog()
{
	{   // Position the sliders and text box controls.
		const int WD = 100;  // Width of each slider column
		const int TEXT_BOTTOM = 100;
		int i = 0;
		for (CSliderCtrl& slider : sliderControls) {
			slider.Create(WS_CHILD | WS_VISIBLE | TBS_VERT, CRect(i * WD, TEXT_BOTTOM, (i + 1) * WD, 280), this, SLIDER_CONTROL_BASE_ID + i);
			slider.SetRange(0, 1000);
			i += 1;
		}
		i = 0;
		for (CStatic& textControl : textControls) {
			CString txt;
			txt.Format(L"Slider %d", i);
			textControl.Create(txt, WS_CHILD | WS_VISIBLE, CRect(i * WD, 10, (i + 1) * WD, TEXT_BOTTOM), this, STATIC_CONTROL_BASE_ID + 1);
			textControl.SetWindowTextW(txt);
			i += 1;
		}
	}
	CDialogEx::OnInitDialog();


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
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon


	createSessionManager();
	updateEverythingFromOS();
	SetTimer(DEAD_SESSION_TIMER_ID, 433, NULL);
	SetTimer(AUDIO_METER_TIMER_ID, 83, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CAudioSessionsMixerCDlg::updateSlidersFromSessions() {
	bool stillConnected[SLIDER_COUNT];
	for (int i = 0; i < SLIDER_COUNT; ++i) stillConnected[i] = false;


	// Validate that existing sliders still make sense to be connected.
	for (Slider& slider : sliders) {
		if (!slider.connected) continue;
		int i = findSessionIndexBySid(slider.sid);
		if (i < 0) {
			TRACE("Slider was connected but didn't match an audio session. disconnecting. %ls", slider.sid);
			slider.connected = false;
			continue;
		}
		//if (!isSessionActive(*audioSessions[i])) slider.connected = false;
	}

	float maxSliderVolume = 0;  // max volume across all sliders.
	for (const Slider& slider : sliders) {
		maxSliderVolume = max(maxSliderVolume, slider.connected ? slider.volumeFromSystem : 0);
	}

	// Connect sliders to audio dessions.
	// Iterate backwards as we may be removing dead sessions.
	for (int j = int(audioSessions.size()) - 1; j >= 0; --j) {
		const auto& session = audioSessions[j];

		const LPWSTR& sid = session->sid;
		if (sid == NULL) {
			TRACE("NULL SID???");
			continue;
		}

		bool isSidUpdate = false;  // Whether we should override the current slider drag position.

		// Scan through sliders to find one with matching ID
		Slider* slider = NULL;
		int i = findSliderIndexBySid(sid);
		if (i >= 0) {
			slider = &(sliders[i]);
			stillConnected[i] = true;
		}

		float volumeFromSystem = 0;
		BOOL mute;
		int hr;
		CHECK_HR(hr = session->pSessionVolumeCtrl->GetMute(&mute));
		if (!mute) {
			CHECK_HR(hr = session->pSessionVolumeCtrl->GetMasterVolume(&volumeFromSystem));
		}

		// new or excess applications shouldn't be allowed to be louder than the loudest app.
		if (maxSliderVolume > 0 && slider == NULL && volumeFromSystem > maxSliderVolume) {
			session->pSessionVolumeCtrl->SetMasterVolume(maxSliderVolume, NULL);
			volumeFromSystem = maxSliderVolume;
		}

		// No matching one? Find a free slot.
		if (slider == NULL) {
			for (int i = 0; i < SLIDER_COUNT; ++i) {
				if (!sliders[i].connected) {
					isSidUpdate = true;
					slider = &(sliders[i]);
					stillConnected[i] = true;
					break;
				}
			}
		}

		// No free slot? Too bad. Ignore it.
		if (slider == NULL) {
			TRACE("Out of sliders for sid %ls\n", sid);
			continue;
		}

		DWORD pid = NULL;
		hr = session->pSessionControl2->GetProcessId(&pid);
		if (hr == 143196173) hr = 0; // AUDCLNT_S_NO_CURRENT_PROCESS for explorer
		CHECK_HR(hr);

		CString label;
		//CHECK_HR(hr = session->pSessionControl2->GetDisplayName(&label));
		label = CString(GetProcName(pid).c_str());
		if (label == "") {
			TRACE("Removing dead session: %ls", sid);
			std::unique_lock<std::shared_mutex> lock(audioSessionsMutex);
			audioSessions.erase(audioSessions.begin() + j);
			slider->connected = false;
			continue;
		}
		else if (label == "[System Process]" || label == "explorer.exe") {
			label = "System";
		}


		if (label.GetLength() > 4) {
			CString extension = label.Right(4);
			if (extension == ".exe") label = label.Left(label.GetLength() - 4);
		}

		if (wcscmp(slider->label, label)) isSidUpdate = true;

		if (label == "") {
			TRACE("bad label: slider pid=%d sid=%ls len=%d", pid, sid);
		}

		if (slider->volumeFromSystem != volumeFromSystem) slider->systemVolumeUpdateTime = time(0);
		if (isSidUpdate)slider->sidUpdateTime = time(0);
		slider->connected = true;
		slider->sid = sid;
		slider->label = label;
		slider->volumeFromSystem = volumeFromSystem;
		slider->volumeIntent = volumeFromSystem;
		slider->vuMeter = 0; // TODO
	}

	// TODO: maybe some smarts around checking to not reuse one that is being actively changed.
	for (int i = 0; i < SLIDER_COUNT; ++i) {
		if (!stillConnected[i]) {
			sliders[i].connected = false;
			sliders[i].sidUpdateTime = time(0);
		}
	}

	SwapSliderToPreferredIndex(L"System", 0);
	SwapSliderToPreferredIndex(L"firefox", 7);
}

void CAudioSessionsMixerCDlg::SwapSliderToPreferredIndex(CString label, int preferredIndex) {
	if (sliders[preferredIndex].label == label) return;
	for (int i = 0; i < SLIDER_COUNT; ++i) {
		if (sliders[i].label != label) continue;

		// swap
		Slider tmp = sliders[preferredIndex];
		sliders[preferredIndex] = sliders[i];
		sliders[i] = tmp;

		// notify that we've done an update on these.
		time_t t = time(0);
		sliders[i].sidUpdateTime = t;
		sliders[i].systemVolumeUpdateTime = t;
		sliders[preferredIndex].sidUpdateTime = t;
		sliders[preferredIndex].systemVolumeUpdateTime = t;
		return;
	}
	TRACE("Preferred label not found: %ls\n", label);
}


void lazyUpdateTextControl(CStatic& textControl, const CString& txt) {
	CString txt2;
	textControl.GetWindowTextW(txt2);
	if (txt == txt2) return;
	textControl.SetWindowTextW(txt);
}
void lazyUpdateSliderControl(CSliderCtrl& sliderControl, int pos) {
	if (pos == sliderControl.GetPos())return;
	sliderControl.SetPos(pos);

}
void CAudioSessionsMixerCDlg::updateControlsFromSliders() {
	for (int i = 0; i < SLIDER_COUNT; ++i) {
		const Slider& slider = sliders[i];
		CSliderCtrl& sliderControl = sliderControls[i];
		CStatic& textControl = textControls[i];
		float volume = slider.volumeIntent;
		if (slider.systemVolumeUpdateTime > slider.dragEndTime) volume = slider.volumeFromSystem;
		if (slider.dragStartTime > slider.dragEndTime) volume = slider.volumeIntent;
		if (slider.sidUpdateTime > slider.dragStartTime) volume = slider.volumeFromSystem;

		// GUI controls
		if (slider.connected) {
			CString txt;
			txt.Format(L"%ls\n\nvuMeter:\n%f", slider.label, slider.vuMeter);
			lazyUpdateTextControl(textControl, txt);

			int pos = int(volume * sliderControl.GetRangeMin() + (1.f - volume) * sliderControl.GetRangeMax());
			lazyUpdateSliderControl(sliderControl, pos);
		}
		else {
			lazyUpdateTextControl(textControl, L"-");
			lazyUpdateSliderControl(sliderControl, sliderControl.GetRangeMax());
		}

		// MIDI controls
		if (slider.connected) {
			midiController.setLabel(i, slider.label);
			midiController.setSliderPos(i, volume);
		}
		else {
			midiController.setLabel(i, L"");
			midiController.setSliderPos(i, 0);
		}
	}
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

void CAudioSessionsMixerCDlg::OnDestroy() {
	// turn off the lights.
	for (int i = 0; i < SLIDER_COUNT; ++i) {
		midiController.setLabel(i, L"");
		midiController.setAudioMeter(i, 0);
		// leave the slider positions as they are.
	}
	CDialogEx::OnDestroy();
}

// The system calls this function to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CAudioSessionsMixerCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CAudioSessionsMixerCDlg::allSessionAlive() {
	std::shared_lock<std::shared_mutex> lock(audioSessionsMutex);
	for (const auto& session : audioSessions) {
		if (!session) {
			TRACE("WTF\n");
			continue;
		}
		DWORD pid = NULL;
		int hr;
		hr = session->pSessionControl2->GetProcessId(&pid);
		if (hr == 143196173) hr = 0; // AUDCLNT_S_NO_CURRENT_PROCESS expected for explorer
		CHECK_HR(hr);
		if (GetProcName(pid) == L"") {  // AUDCLNT_S_NO_CURRENT_PROCESS 
			// TRACE("Detected dead session: hr=%d sid=%ls\n", hr, session->sid);
			return false;
		}
	}
	return true;
}

void CAudioSessionsMixerCDlg::OnTimer(UINT_PTR nIdEvent)
{
	if (nIdEvent == DEAD_SESSION_TIMER_ID) {
		if (!allSessionAlive()) {
			updateEverythingFromOS();
		}
	}
	else if (nIdEvent == AUDIO_METER_TIMER_ID) {
		std::shared_lock<std::shared_mutex> lock(audioSessionsMutex);  // don't change the meaning of `j`.
		for (int i = 0; i < SLIDER_COUNT; ++i) {
			if (!sliders[i].connected) continue;
			int j = findSessionIndexBySid(sliders[i].sid);
			if (j < 0) continue;
			auto& session = audioSessions[j];
			if (session->state == AudioSessionState::AudioSessionStateActive) {
				int hr;
				float peak;
				CHECK_HR(hr = session->pAudioMeterInformation->GetPeakValue(&peak));
				midiController.setAudioMeter(i, peak);
			}
			else {
				midiController.setAudioMeter(i, 0);
			}
		}
	}
}


void CAudioSessionsMixerCDlg::updateSessionsFromManager()
{
	HRESULT hr;

	// Get the current list of sessions.
	CHECK_HR(hr = pSessionManager->GetSessionEnumerator(&pSessionList));

	// Get the session count.
	int sessionCount = 0;
	CHECK_HR(hr = pSessionList->GetCount(&sessionCount));

	for (int i = 0; i < sessionCount; i++)
	{
		// Get the <n>th session->
		IAudioSessionControl* pSessionControl;
		IAudioSessionControl2* pSessionControl2;
		IAudioMeterInformation* pAudioMeterInformation;

		CHECK_HR(hr = pSessionList->GetSession(i, &pSessionControl));
		// pSessionControl->AddRef();
		// Get the extended session control interface pointer.
		CHECK_HR(hr = pSessionControl->QueryInterface(
			__uuidof(IAudioSessionControl2), (void**)&pSessionControl2));
		CHECK_HR(hr = pSessionControl->QueryInterface(
			__uuidof(IAudioMeterInformation), (void**)&pAudioMeterInformation));

		LPWSTR sid;
		CHECK_HR(hr = pSessionControl2->GetSessionInstanceIdentifier(&sid));
		if (findSessionIndexBySid(sid) >= 0) {
			continue; // we already have a CAudioSession for this.
		}

		DWORD pid = NULL;
		int hr;
		hr = pSessionControl2->GetProcessId(&pid);
		if (hr == 143196173) hr = 0; // AUDCLNT_S_NO_CURRENT_PROCESS for explorer
		CHECK_HR(hr);
		CString label;
		//CHECK_HR(hr = session->pSessionControl2->GetDisplayName(&label));
		label = CString(GetProcName(pid).c_str());
		if (label == "") {
			//TRACE("Found session already dead: %ls\n", sid);
			continue;
		}


		std::unique_ptr<CAudioSession> session = std::make_unique<CAudioSession>();
		session->sid = sid;
		session->pSessionControl = pSessionControl;
		session->pSessionControl2 = pSessionControl2;
		session->pAudioMeterInformation = pAudioMeterInformation;
		session->eventListener = std::make_unique<CAudioSessionEvents>(sid, (IDomsAudioSessionEvents*)(this));
		CHECK_HR(hr = pSessionControl->RegisterAudioSessionNotification(session->eventListener.get()));
		CHECK_HR(hr = pSessionControl->GetState(&session->state));


		// get session volume control
		CHECK_HR(hr = session->pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
			(void**)&session->pSessionVolumeCtrl));


		//so getting process id and then its name for reference of its session
		//DWORD id = NULL;
		//CHECK_HR(hr = session->pSessionControl2->GetProcessId(&id));//audio session owner process id  

		{
			std::unique_lock<std::shared_mutex> lock(audioSessionsMutex);
			audioSessions.push_back(std::move(session));
		}

		//CString str = L"";
		//HWND hwndo = NULL;
		//int i=GetWindowTextA(GetWindowHandleFromProcessID(id), LPSTR(str.GetString()), NULL);
	}
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

	pSessionManager->RegisterSessionNotification(&pSessionNotifications);
}

CAudioSessionsMixerCDlg::~CAudioSessionsMixerCDlg()
{
	SAFE_RELEASE(pSessionList);
}


void CAudioSessionsMixerCDlg::OnVolumeIntent(const Slider& slider) {

	if (!slider.connected) {
		TRACE("OnVolumeIntent should not be called with unconnected slider!\n");
		return;
	}
	if (slider.sidUpdateTime >= slider.dragStartTime) {
		TRACE("Ignoring user intent due to system change on slider %ls\n", slider.sid);
		return;
	}
	int i = findSessionIndexBySid(slider.sid);
	if (i < 0) {
		TRACE("OnVolumeIntent could not find corresponding session for slider %ls\n", slider.sid);
		return;
	}
	audioSessions[i]->pSessionVolumeCtrl->SetMasterVolume(slider.volumeIntent, NULL);
}

void CAudioSessionsMixerCDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CSliderCtrl* sliderControl = reinterpret_cast<CSliderCtrl*>(pScrollBar);

	// Check which slider sent the notification  
	for (int i = 0; i < SLIDER_COUNT; ++i) {
		if (sliderControl == &(sliderControls[i])) {
			Slider& slider = sliders[i];
			if (slider.connected) {
				float volumeIntent = 1 - float(sliderControl->GetPos() - sliderControl->GetRangeMin()) / float(sliderControl->GetRangeMax() - sliderControl->GetRangeMin());
				slider.volumeIntent = volumeIntent;
				slider.dragStartTime = time(0);  // FIXME: This should only be set on mouse down.
				slider.dragEndTime = time(0);
				OnVolumeIntent(slider);
			}
			else if (nPos != 0) {
				sliderControl->ClearSel();
				sliderControl->SetPos(sliderControl->GetRangeMax());
			}
			break;
		}
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}




// These methods implement IDomsAudioSessionEvents.




HRESULT STDMETHODCALLTYPE CAudioSessionsMixerCDlg::OnSimpleVolumeChanged(
	const LPWSTR& sid,
	float NewVolume,
	BOOL NewMute,
	LPCGUID EventContext) {
	int i = findSliderIndexBySid(sid);
	if (i < 0) {
		TRACE("Got a OnSimpleVolumeChanged unmatched slider: %ls", sid);
		return S_OK;  // This is expected if we have more than SLIDER_COUNT active sessions.
	}
	sliders[i].systemVolumeUpdateTime = time(0);
	sliders[i].volumeFromSystem = NewMute ? 0 : NewVolume;
	updateControlsFromSliders();
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CAudioSessionsMixerCDlg::OnStateChanged(
	const LPWSTR& sid,
	AudioSessionState NewState) {
	std::shared_lock<std::shared_mutex> lock(audioSessionsMutex);
	int i = findSessionIndexBySid(sid);
	if (i >= 0) {
		audioSessions[i]->state = NewState;
	}
	else {
		TRACE("session not found: %ls", sid);
	}

	return S_OK;
}
HRESULT STDMETHODCALLTYPE CAudioSessionsMixerCDlg::OnSessionDisconnected(
	const LPWSTR& sid,
	AudioSessionDisconnectReason DisconnectReason) {
	TRACE("Dom has never seen OnSessionDisconnected() being fired. why is it working now?");
	int i = findSessionIndexBySid(sid);
	if (i >= 0) {
		std::unique_lock<std::shared_mutex> lock(audioSessionsMutex);
		audioSessions.erase(audioSessions.begin() + i);
	}
	else {
		TRACE("session disconnected twice? %ls", sid);
	}
	updateEverythingFromOS();
	return S_OK;
}
HRESULT CAudioSessionsMixerCDlg::OnSessionCreated(IAudioSessionControl* pNewSession) {
	updateEverythingFromOS();
	return S_OK;
}


// Things for finding stuff by sid's.
int CAudioSessionsMixerCDlg::findSessionIndexBySid(const LPWSTR& sid) {
	std::shared_lock<std::shared_mutex> lock(audioSessionsMutex);

	size_t size = audioSessions.size();
	for (int i = 0; i < size; ++i) {
		int j = (lastFoundSessionIndex + i) % size;
		const std::unique_ptr<CAudioSession>& session = audioSessions[j];
		if (wcscmp(session->sid, sid) == 0) {
			lastFoundSessionIndex = j;
			return j;
		}
	}
	return -1;
}
int CAudioSessionsMixerCDlg::findSliderIndexBySid(const LPWSTR& sid) {
	for (int i = 0; i < SLIDER_COUNT; ++i) {
		int j = (lastFoundSliderIndex + i) % SLIDER_COUNT;
		const Slider& slider = sliders[j];
		if (slider.connected && wcscmp(slider.sid, sid) == 0) {
			lastFoundSliderIndex = j;
			return j;
		}
	}
	return -1;
}


// Implement IMidiControllerEventReceiver

void CAudioSessionsMixerCDlg::OnMidiControllerDragged(int sliderIndex, float volume) {
	//TRACE("OnMidiControllerDragged(%d, %f)\n", sliderIndex, volume);
	Slider& slider = sliders[sliderIndex];
	if (!slider.connected) {
		midiController.setSliderPos(sliderIndex, 0);  // reset to off if the user tries to turn on a unconnected slider.
		return;
	}
	slider.volumeIntent = volume;
	OnVolumeIntent(slider);
}
void CAudioSessionsMixerCDlg::OnMidiControllerTouch(int sliderIndex, bool down) {
	//TRACE("OnTouch(sliderIndex=%d, %s)\n", sliderIndex, down ? "down" : "up");
	Slider& slider = sliders[sliderIndex];
	if (down) {
		slider.dragStartTime = time(0);
	}
	else {
		slider.dragEndTime = time(0);
	}
}
void CAudioSessionsMixerCDlg::OnMidiControllerKnob(int sliderIndex, bool clockwise) {
	TRACE("YO %d\n", clockwise);
	std::swap(
		sliders[sliderIndex],
		sliders[(sliderIndex + (clockwise ? 1 : -1) + SLIDER_COUNT) % SLIDER_COUNT]
	);
	updateControlsFromSliders();
}