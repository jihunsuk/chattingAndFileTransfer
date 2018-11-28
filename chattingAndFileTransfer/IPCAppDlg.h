// IPCAppDlg.h : header file
//

#if !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
#define AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_

#include "LayerManager.h"	// Added by ClassView
#include "ChatAppLayer.h"	// Added by ClassView
#include "EthernetLayer.h"	// Added by ClassView
#include "TCPLayer.h"	// Added by ClassView
#include "IPLayer.h"	// Added by ClassView
#include "FileLayer.h"	// Added by ClassView
#include "NILayer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg dialog

class CIPCAppDlg 
: public CDialog, 
public CBaseLayer
{
	// Construction
public:
	CIPCAppDlg(CWnd* pParent = NULL);	// standard constructor


	CWinThread*	m_RecvThread1;
	CWinThread*	m_RecvThread2;
	CWinThread*	m_RecvThread3;
	static UINT	ReceiveThread(LPVOID pParam);
	static UINT	ReceiveFileThread(LPVOID pParam);
	static UINT FileThread(LPVOID pParam);
	static UINT	ReceiveTCPThread(LPVOID pParam);
	// Dialog Data
	//{{AFX_DATA(CIPCAppDlg)
	enum { IDD = IDD_IPC_DIALOG };
	CProgressCtrl	m_progress;
	CComboBox	m_NICSet;
	CListBox	m_ListChat;
	CString	m_stMessage;
	CString	m_stSrcAdd;
	CString	m_stDstAdd;
	CString	m_strFile;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPCAppDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	
	// Generated message map functions
	//{{AFX_MSG(CIPCAppDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSendMessage();
	afx_msg void OnButtonAddrSet();
	afx_msg void OnButtonFileserch();
	afx_msg void OnButtonFiletrans();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
public:
	BOOL			ConvertHex(CString cs, unsigned char* hex);
	BOOL			Receive( unsigned char* ppayload );
	inline void		SendData( );

	void TransferdButtonChange();
	
private:
	
	CString payload;
	
	CLayerManager	m_LayerMgr;
	int				m_nAckReady;
	enum {			IPC_INITIALIZING, 
		IPC_READYTOSEND, 
		IPC_WAITFORACK,
		IPC_ERROR,
		IPC_ADDR_SET,
		IPC_ADDR_RESET,
		NOTSETADDRESS,
		IPC_SET_FileOn,
		IPC_SET_FileOff} ;
	
	
	void			SetDlgState( int state );
	inline void		EndofProcess( );
	inline void		SetRegstryMessage( );
	
	BOOL			m_bSendReady;

	unsigned char*	m_SrcMacAddress;	// 시작지의 Mac주소를 가리키는 포인터
										// 실제배열은 NI계층에 존재
	unsigned char	m_DstMacAddress[6];	// 목적지의 Mac주소
	// Object App
	CChatAppLayer*	m_ChatApp ;
	CEthernetLayer* m_Ether;
	CFileLayer*		m_File;
	CNILayer* m_NI ;	
	
public:
	afx_msg void OnBnClickedButton3();
//	afx_msg void OnDropdownTestPacket(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedTestPacket();
	BOOL			FileSended;//sended check
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPCAPPDLG_H__FE9C37CB_4C90_4772_99AD_50A9B24BCE62__INCLUDED_)
