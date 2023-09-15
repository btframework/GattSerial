// GettSerialClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GettSerialClient.h"
#include "GettSerialClientDlg.h"

const GUID SERIAL_SERVICE_UUID = { 0x6E40FEC1, 0xB5A3, 0xF393, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };

const GUID RX_CHARACTERISTIC_UUID = { 0x6E40FEC2, 0xB5A3, 0xF393, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };
const GUID TX_CHARACTERISTIC_UUID = { 0x6E40FEC3, 0xB5A3, 0xF393, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E };

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CHexEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

CHexEdit::CHexEdit()
	: CEdit()
{
}

CHexEdit::~CHexEdit()
{
}

void CHexEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString Valid(_T("0123456789ABCDEFabcdef"));
	if (Valid.Find(nChar) != -1 || nChar == 0x08)
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}


CString IntToHex(const int i)
{
	CString s;
	s.Format(_T("%.8X"), i);
	return s;
}

CString IntToHex(const __int64 i)
{
	CString s;
	s.Format(_T("%.4X%.8X"), static_cast<INT32>((i >> 32) & 0x00000FFFF),
		static_cast<INT32>(i) & 0xFFFFFFFF);
	return s;
}

CString IntToStr(const int i)
{
	CString s;
	s.Format(_T("%d"), i);
	return s;
}

CString IntToHex(const unsigned char i)
{
	CString s;
	s.Format(_T("%.2X"), i);
	return s;
}

// CGettSerialClientDlg dialog




CGettSerialClientDlg::CGettSerialClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGettSerialClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGettSerialClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, btConnect);
	DDX_Control(pDX, IDC_BUTTON_DISCONNECT, btDisconnect);
	DDX_Control(pDX, IDC_BUTTON_SEND, btSend);
	DDX_Control(pDX, IDC_EDIT_VALUE, edValue);
	DDX_Control(pDX, IDC_LIST_LOG, lbLog);
}

BEGIN_MESSAGE_MAP(CGettSerialClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CGettSerialClientDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CGettSerialClientDlg::OnBnClickedButtonConnect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CGettSerialClientDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CGettSerialClientDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CGettSerialClientDlg message handlers

BOOL CGettSerialClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	btDisconnect.EnableWindow(FALSE);
	btSend.EnableWindow(FALSE);

	edValue.SetWindowText(_T("11223344"));

	__hook(&CwclBluetoothLeBeaconWatcher::OnStarted, &Watcher, &CGettSerialClientDlg::WatcherStarted);
	__hook(&CwclBluetoothLeBeaconWatcher::OnStopped, &Watcher, &CGettSerialClientDlg::WatcherStopped);
	__hook(&CwclBluetoothLeBeaconWatcher::OnAdvertisementUuidFrame, &Watcher, &CGettSerialClientDlg::WatcherAdvertisementUuidFrame);

	__hook(&CwclBluetoothManager::AfterOpen, &Manager, &CGettSerialClientDlg::ManagerAfterOpen);
	__hook(&CwclBluetoothManager::OnClosed, &Manager, &CGettSerialClientDlg::ManagerClosed);

	__hook(&CwclGattClient::OnDisconnect, &Client, &CGettSerialClientDlg::ClientDisconnect);
	__hook(&CwclGattClient::OnMaxPduSizeChanged, &Client, &CGettSerialClientDlg::ClientMaxPduSizeChanged);
	__hook(&CwclGattClient::OnConnect, &Client, &CGettSerialClientDlg::ClientConnect);
	__hook(&CwclGattClient::OnCharacteristicChanged, &Client, &CGettSerialClientDlg::ClientCharacteristicChanged);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGettSerialClientDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGettSerialClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGettSerialClientDlg::Trace(const CString& Str)
{
	lbLog.AddString(Str);
}

void CGettSerialClientDlg::Trace(const CString& Str, const int Error)
{
	Trace(Str + _T(": 0x") + IntToHex(Error));
}

void CGettSerialClientDlg::OnBnClickedButtonClear()
{
	lbLog.ResetContent();
}

void CGettSerialClientDlg::OnBnClickedButtonConnect()
{
	if (Manager.Active)
		AfxMessageBox(_T("Already connected"));
	else
	{
		btConnect.EnableWindow(FALSE);
		btDisconnect.EnableWindow(TRUE);
		
		Trace(_T("Opening Bluetooth Manager"));
		int Res = Manager.Open();
		if (Res != WCL_E_SUCCESS)
			Trace(_T("Bluetooth Manager open failed"), Res);
		else
		{
			Trace(_T("Get working LE radio"));
			CwclBluetoothRadio* Radio;
			Res = Manager.GetLeRadio(Radio);
			if (Res != WCL_E_SUCCESS)
				Trace(_T("Get working LE radio failed"), Res);
			else
			{
				Trace(_T("Start beacon watcher"));
				Res = Watcher.Start(Radio);
				if (Res != WCL_E_SUCCESS)
					Trace(_T("Start Beacon Watcher failed"));
			}
			
			if (Res != WCL_E_SUCCESS)
				Manager.Close();
		}
	}
}

void CGettSerialClientDlg::WatcherStarted(void* Sender)
{
	UNREFERENCED_PARAMETER(Sender);

	Trace(_T("Beacon watcher started"));
}

void CGettSerialClientDlg::WatcherStopped(void* Sender)
{
	UNREFERENCED_PARAMETER(Sender);

	Trace(_T("Beacon watcher stopped"));
}

void CGettSerialClientDlg::WatcherAdvertisementUuidFrame(void* Sender,
	const __int64 Address, const __int64 Timestamp, const char Rssi,
	const GUID& Uuid)
{
	UNREFERENCED_PARAMETER(Sender);
	UNREFERENCED_PARAMETER(Timestamp);
	UNREFERENCED_PARAMETER(Rssi);

	// Additionally you can filter device by MAC or somehow else.
	if (memcmp(&Uuid, &SERIAL_SERVICE_UUID, sizeof(GUID)) == 0)
	{
		Trace(_T("Device found: ") + IntToHex(Address));
		
		// Get radio here! After stop it will not be available!
		CwclBluetoothRadio* Radio = Watcher.Radio;
		Watcher.Stop();
		
		Trace(_T("Try to connect"));
		Client.Address = Address;
		int Res = Client.Connect(Radio);
		if (Res != WCL_E_SUCCESS)
		{
			Trace(_T("Start connecting failed"), Res);
			Manager.Close();
		}
	}
}

void CGettSerialClientDlg::OnDestroy()
{
	CDialog::OnDestroy();

	Manager.Close();

	__unhook(&Watcher);
	__unhook(&Manager);
	__unhook(&Client);
}

void CGettSerialClientDlg::ManagerAfterOpen(void* Sender)
{
	Trace(_T("Bluetooth Manager opened"));
}

void CGettSerialClientDlg::ManagerClosed(void* Sender)
{
	Trace(_T("Bluetooth Manager closed"));
	
	btConnect.EnableWindow(TRUE);
	btDisconnect.EnableWindow(FALSE);
	btSend.EnableWindow(FALSE);
}

void CGettSerialClientDlg::OnBnClickedButtonDisconnect()
{
	if (!Manager.Active)
		AfxMessageBox(_T("Not connected"));
	else
		Manager.Close();
}

void CGettSerialClientDlg::ClientDisconnect(void* Sender,
	const int Reason)
{
	Trace(_T("Client disconnected"), Reason);
	Manager.Close();
}

void CGettSerialClientDlg::ClientMaxPduSizeChanged(void* Sender)
{
	unsigned short Size;
	int Res = Client.GetMaxPduSize(Size);
	if (Res == WCL_E_SUCCESS)
		Trace(_T("PDU size changed: ") + IntToStr(Size));
	else
		Trace(_T("PDU size changed"));
}

void CGettSerialClientDlg::ClientConnect(void* Sender, const int Error)
{
	if (Error != WCL_E_SUCCESS)
	{
		Trace(_T("Connect failed"), Error);
		Manager.Close();
	}
	else
	{
		Trace(_T("Connected"));
		
		wclGattUuid Uuid;
		Uuid.IsShortUuid = false;
		
		Trace(_T("Find service"));
		Uuid.LongUuid = SERIAL_SERVICE_UUID;
		wclGattService Service;
		int Res = Client.FindService(Uuid, Service);
		if (Res != WCL_E_SUCCESS)
			Trace(_T("Unable to find service"), Res);
		else
		{
			Trace(_T("Find TX characteristic"));
			Uuid.LongUuid = TX_CHARACTERISTIC_UUID;
			Res = Client.FindCharacteristic(Service, Uuid, TxChar);
			if (Res != WCL_E_SUCCESS)
				Trace(_T("TX characteristic not found"), Res);
			else
			{
				Trace(_T("Find RX characteristic"));
				Uuid.LongUuid = RX_CHARACTERISTIC_UUID;
				wclGattCharacteristic RxChar;
				Res = Client.FindCharacteristic(Service, Uuid, RxChar);
				if (Res != WCL_E_SUCCESS)
					Trace(_T("RX charaterisitc not found"), Res);
				else
				{
					Trace(_T("Subscribe to notifications"));
					Res = Client.SubscribeForNotifications(RxChar);
					if (Res != WCL_E_SUCCESS)
						Trace(_T("Subscribe failed"), Res);
					else
					{
						Trace(_T("Connection completed"));
						btSend.EnableWindow(TRUE);
					}
				}
			}
		}
		
		if (Res != WCL_E_SUCCESS)
		{
			Trace(_T("Wrong device"));
			Client.Disconnect();
		}
	}
}

void CGettSerialClientDlg::ClientCharacteristicChanged(void* Sender,
	const unsigned short Handle, const unsigned char* const Value,
	const unsigned long Length)
{
	Trace(_T("Data received"));
	if (Value == NULL || Length == 0)
		Trace(_T("  Empty data"));
	else
	{
		CString Str = _T("");
		for (unsigned long i = 0; i < Length; i++)
			Str = Str + IntToHex(Value[i]);
		
		Trace(_T("  Value: ") + Str);
	}
}

void CGettSerialClientDlg::OnBnClickedButtonSend()
{
	if (Client.State != csConnected)
		AfxMessageBox(_T("Not connected"));
	else
	{
		CString Str;
		edValue.GetWindowText(Str);
		if (Str.GetLength() == 0)
			AfxMessageBox(_T("Value can not be empty"));
		else
		{
			if (Str.GetLength() % 2 != 0)
				Str = _T("0") + Str;
			
			unsigned long Length = Str.GetLength() / 2;
			unsigned char* Val = (unsigned char*)malloc(Length);
			if (Val == 0)
				AfxMessageBox(_T("Memory allocation failed"));
			else
			{
				int i = 0;
				int j = 0;
				while (i < Str.GetLength())
				{
					unsigned char b = LOBYTE(LOWORD(_tcstol(Str.Mid(i, 2), NULL, 16)));
					Val[j] = b;
					i += 2;
					j++;
				}

				int Res = Client.WriteCharacteristicValue(TxChar, Val, Length);
				if (Res != WCL_E_SUCCESS)
					Trace(_T("Send failed"), Res);
				else
					Trace(_T("Sent"));
			}
		}
	}
}
