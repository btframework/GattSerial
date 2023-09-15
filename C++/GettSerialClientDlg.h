// GettSerialClientDlg.h : header file
//

#pragma once
#include "afxwin.h"

#include "..\\..\\..\\..\\WCL7\\CPP\\Source\\Bluetooth\\wclBluetooth.h"
#include "..\\..\\..\\..\\WCL7\\CPP\\Source\\Common\\wclErrors.h"

using namespace wclCommon;
using namespace wclCommunication;
using namespace wclBluetooth;


class CHexEdit : public CEdit
{
protected:
	DECLARE_MESSAGE_MAP()

public:
	CHexEdit();
	virtual ~CHexEdit();

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


// CGettSerialClientDlg dialog
class CGettSerialClientDlg : public CDialog
{
// Construction
public:
	CGettSerialClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GETTSERIALCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CButton btConnect;
	CButton btDisconnect;
	CButton btSend;
	CHexEdit edValue;
	CListBox lbLog;

	wclGattCharacteristic TxChar;

	CwclBluetoothManager Manager;
	CwclGattClient Client;
	CwclBluetoothLeBeaconWatcher Watcher;

    void Trace(const CString& Str);
	void Trace(const CString& Str, const int Error);

	void WatcherStarted(void* Sender);
	void WatcherStopped(void* Sender);
	void WatcherAdvertisementUuidFrame(void* Sender, const __int64 Address,
		const __int64 Timestamp, const char Rssi, const GUID& Uuid);

	void ManagerAfterOpen(void* Sender);
	void ManagerClosed(void* Sender);

	void ClientDisconnect(void* Sender, const int Reason);
	void ClientMaxPduSizeChanged(void* Sender);
	void ClientConnect(void* Sender, const int Error);
	void ClientCharacteristicChanged(void* Sender, const unsigned short Handle,
		const unsigned char* const Value, const unsigned long Length);

public:
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonSend();
};
