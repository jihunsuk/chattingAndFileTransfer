// IPCAppDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ipc.h"
#include "IPCAppDlg.h"
#include "AboutboxDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg dialog

CIPCAppDlg::CIPCAppDlg(CWnd* pParent /*=NULL*/)
: CDialog(CIPCAppDlg::IDD, pParent),
CBaseLayer("ChatDlg"),
m_bSendReady(FALSE),
m_nAckReady(-1)
{
	FileSended=FALSE;
	//{{AFX_DATA_INIT(CIPCAppDlg)
	m_stMessage = _T("");
	m_stSrcAdd = _T("");
	m_stDstAdd = _T("");
	m_strFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_LayerMgr.AddLayer(new CChatAppLayer("ChatApp"));
	m_LayerMgr.AddLayer(new CEthernetLayer("Ethernet"));
	m_LayerMgr.AddLayer(new CIPLayer("Ip"));
	m_LayerMgr.AddLayer(new CTCPLayer("Tcp"));
	m_LayerMgr.AddLayer(new CFileLayer("File"));
	m_LayerMgr.AddLayer(new CNILayer("NI"));
	m_LayerMgr.AddLayer(this);


	//connet Layers
	m_LayerMgr.ConnectLayers("NI ( *Ethernet ( *Ip ( *Tcp ( *ChatApp ( *ChatDlg )  *File ( +ChatDlg ) ) ) ) ) )");

	m_NI = (CNILayer*) m_LayerMgr.GetLayer("NI");
	m_Ether = (CEthernetLayer*) m_LayerMgr.GetLayer("Ethernet");
	m_ChatApp = (CChatAppLayer*) mp_UnderLayer;
	m_File = (CFileLayer*) m_LayerMgr.GetLayer("File");
}

void CIPCAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPCAppDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_COMBO_NIC, m_NICSet);
	DDX_Control(pDX, IDC_LIST_CHAT, m_ListChat);
	DDX_Text(pDX, IDC_EDIT_MSG, m_stMessage);
	DDX_Text(pDX, IDC_EDIT_SRC, m_stSrcAdd);
	DDX_Text(pDX, IDC_EDIT_DST, m_stDstAdd);
	DDX_Text(pDX, IDC_EDIT_FILESEARCH, m_strFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIPCAppDlg, CDialog)
	//{{AFX_MSG_MAP(CIPCAppDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADDR, OnButtonAddrSet)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnSendMessage)
	ON_BN_CLICKED(IDC_BUTTON_FILESERCH, OnButtonFileserch)
	ON_BN_CLICKED(IDC_BUTTON_FILETRANS, OnButtonFiletrans)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	//ON_NOTIFY(BCN_DROPDOWN, IDC_TEST_PACKET, &CIPCAppDlg::OnDropdownTestPacket)
	ON_BN_CLICKED(IDC_TEST_PACKET, &CIPCAppDlg::OnBnClickedTestPacket)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPCAppDlg message handlers

BOOL CIPCAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	SetDlgState(IPC_INITIALIZING);

	for (int i = 0; i < m_NI->getAdapterNum(); i++){
		m_NICSet.AddString(m_NI->getAdapterName(i));
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CIPCAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		if (MessageBox("Are you sure ?",
			"Question",
			MB_YESNO | MB_ICONQUESTION)
			== IDNO)
			return;
		else EndofProcess();
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIPCAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIPCAppDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
//event by message Send button
void CIPCAppDlg::OnSendMessage()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (!m_stMessage.IsEmpty())
	{
		SendData();
		m_stMessage = "";

		(CEdit*) GetDlgItem(IDC_EDIT_MSG)->SetFocus();

	}

	UpdateData(FALSE);
}

void CIPCAppDlg::OnButtonAddrSet()
{
	UpdateData(TRUE);

	if (!m_bSendReady){
		// if not Setup, make m_bSendReady = TRUE
		m_bSendReady = TRUE;
		SetDlgState(IPC_ADDR_SET);

		m_NI->setAdapterNum(m_NICSet.GetCurSel());
		m_NI->setMacAddress();
		m_SrcMacAddress = m_NI->getMacAddress();
		m_stSrcAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
			m_SrcMacAddress[0], m_SrcMacAddress[1], m_SrcMacAddress[2],
			m_SrcMacAddress[3], m_SrcMacAddress[4], m_SrcMacAddress[5]);

		if (m_stDstAdd == "")//BroadCast
			m_stDstAdd = "FFFFFFFFFFFF";

		//save des
		m_stDstAdd.MakeLower();	// change LowwerBound
		ConvertHex(m_stDstAdd, m_DstMacAddress);	// CString change to Hex
		m_stDstAdd.Format("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
			m_DstMacAddress[0], m_DstMacAddress[1], m_DstMacAddress[2],
			m_DstMacAddress[3], m_DstMacAddress[4], m_DstMacAddress[5]);

		m_Ether->SetSourceAddress(m_SrcMacAddress);
		m_Ether->SetDestinAddress(m_DstMacAddress);

		//start Thread
		m_RecvThread1 = ::AfxBeginThread(CIPCAppDlg::ReceiveThread, this);
		m_RecvThread2 = ::AfxBeginThread(CIPCAppDlg::ReceiveFileThread, this);
		m_RecvThread3 = ::AfxBeginThread(CIPCAppDlg::ReceiveTCPThread, this);
	}
	else {
		// rebutton make not Ready
		m_bSendReady = FALSE;
		SetDlgState(IPC_ADDR_RESET);
	}
	UpdateData(FALSE);

}
BOOL CIPCAppDlg::ConvertHex(CString cs, unsigned char* hex)//change string to hex
{
	int i;
	char* srcStr = cs.GetBuffer(0);

	for (i = 0; i<12; i++){
		// error
		if (srcStr[i] < '0' || (srcStr[i] > '9' && srcStr[i] < 'a') || srcStr[i] > 'f')
			return FALSE;
	}
	for (i = 0; i<12; i = i + 2){
		hex[i / 2] = (((srcStr[i] > '9') ? (srcStr[i] - 87) : (srcStr[i] - '0')) << 4 |
			((srcStr[i + 1] > '9') ? (srcStr[i + 1] - 87) : (srcStr[i + 1] - '0')));
	}
	return TRUE;
}
//receive Chat Thread
UINT CIPCAppDlg::ReceiveThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;

	//if m_bSendReady is on, start Thread
	while (obj->m_bSendReady == TRUE)
	{
		CBaseLayer* bLayer;
		bLayer = obj->m_LayerMgr.GetLayer("ChatApp");
		unsigned char *ppayload = bLayer->Receive(0);
		if (ppayload != NULL)
			obj->Receive(ppayload);
	}
	return 0;
}
//send FileThread
UINT CIPCAppDlg::FileThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;

	obj->m_File->Send((unsigned char*) obj->m_strFile.GetBuffer(0), obj->m_strFile.GetLength(), FILE_TYPE);
	
	obj->FileSended=FALSE;//init
	obj->TransferdButtonChange();
	return 0;
}
//	CEdit*		pFTButton = (CEdit*) GetDlgItem(IDC_BUTTON_FILETRANS);

void CIPCAppDlg::TransferdButtonChange(){//change File Transferd change ButtonChange
	CEdit*		pFTButton = (CEdit*) GetDlgItem(IDC_BUTTON_FILETRANS);
	pFTButton->SetWindowText("전송");
}

//receive Packet to TCPLayer by NILayer
UINT CIPCAppDlg::ReceiveTCPThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;

	//if m_bSendReady is on, start Thread
	while (obj->m_bSendReady == TRUE)
	{
		CBaseLayer* bLayer;
		bLayer = obj->m_LayerMgr.GetLayer("Tcp");
		bLayer->Receive(2);//Thread num
	}

	return 0;
}
//receive File Thread
UINT CIPCAppDlg::ReceiveFileThread(LPVOID pParam)
{
	CIPCAppDlg* obj = (CIPCAppDlg*) pParam;

	//if m_bSendReady is on, start Thread
	while (obj->m_bSendReady == TRUE)
	{
		CBaseLayer* bLayer;
		bLayer = obj->m_LayerMgr.GetLayer("File");
		bLayer->Receive(1);//Thread num
	}

	return 0;
}

void CIPCAppDlg::SendData()
{
	int len;

	char TempString[23];
	memset(TempString, '\0', 23);

	CString MsgHeader;
	MsgHeader.Format("Send >> ");
	int index = m_ListChat.AddString(MsgHeader);
	m_ListChat.SetCurSel(index);
	len = 0;

	if (m_stMessage.GetLength() > 30){//printing 30 Character(line) my display
		while (1){
			int index = m_ListChat.AddString(m_stMessage.Mid(len, 30));
			m_ListChat.SetCurSel(index);
			len += 30;
			if (m_stMessage.GetLength() - len < 30){
				index = m_ListChat.AddString(m_stMessage.Mid(len, m_stMessage.GetLength() - len));
				m_ListChat.SetCurSel(index);
				break;
			}
		}
	}
	else{
		int index = m_ListChat.AddString(m_stMessage);
		m_ListChat.SetCurSel(index);
	}

	len = 0;

	//////////////////////////////segemantation APP_DATA
	if (m_stMessage.GetLength() > APP_DATA_SIZE){//bigger
		while (1){
			if(len == 0){
				this->mp_UnderLayer->Send((unsigned char*) (LPCTSTR) m_stMessage.Mid(len, APP_DATA_SIZE), APP_DATA_SIZE, CHAT_TYPE, CHAT_REC);
			len += APP_DATA_SIZE;
			}else{
				if (m_stMessage.GetLength() - len < APP_DATA_SIZE){//last
				this->mp_UnderLayer->Send((unsigned char*) (LPCTSTR) m_stMessage.Mid(len, m_stMessage.GetLength() - len), m_stMessage.GetLength() - len, CHAT_TYPE, CHAT_NOTREC);
				break;
			}
			this->mp_UnderLayer->Send((unsigned char*) (LPCTSTR) m_stMessage.Mid(len, APP_DATA_SIZE), APP_DATA_SIZE, CHAT_TYPE, CHAT_NOTREC);
			len += APP_DATA_SIZE;
			}		
		}
	}
	else{//lower
		this->mp_UnderLayer->Send((unsigned char*) (LPCTSTR) m_stMessage, m_stMessage.GetLength(), CHAT_TYPE, CHAT_REC);
	}
	//////////////////////////////
}

BOOL CIPCAppDlg::Receive(unsigned char *ppayload)//Chat
{
	CString message;
	message.Format("%s", ppayload);

	if (!message.Compare("Test Packet")) {
		AboutboxDialog dlg;
		dlg.DoModal();
	}
	else {
		if (ppayload[APP_DATA_SIZE + 1] == 1) {//Chat
			message.Format("Receive >>");//message.addstring "header"
			int index = m_ListChat.AddString(message);
			m_ListChat.SetCurSel(index);
		}

		message.Format("%s", ppayload);

		int len = 0;
		if (message.GetLength() > 30) {//printing 30 Character(line)
			while (1) {
				int index = m_ListChat.AddString(message.Mid(len, 30));
				m_ListChat.SetCurSel(index);
				len += 30;
				if (message.GetLength() - len < 30) {
					index = m_ListChat.AddString(message.Mid(len, message.GetLength() - len));
					m_ListChat.SetCurSel(index);
					break;
				}
			}
		}
		else {
			int index = m_ListChat.AddString(message);
			m_ListChat.SetCurSel(index);
		}
	}

	return TRUE;


	//	MessageBox("패킷 도착함");
	//	return TRUE;
}

BOOL CIPCAppDlg::PreTranslateMessage(MSG* pMsg)
{

	// TODO: Add your specialized code here and/or call the base class
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_RETURN://process enter is Send Mesaage
			if (::GetDlgCtrlID(::GetFocus()) == IDC_EDIT_MSG)
				OnSendMessage();					return FALSE;
		case VK_ESCAPE: return FALSE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CIPCAppDlg::SetDlgState(int state)
{
	UpdateData(TRUE);

	CButton*	pSendButton = (CButton*) GetDlgItem(IDC_BUTTON_SEND);
	CButton*	pSetAddrButton = (CButton*) GetDlgItem(IDC_BUTTON_ADDR);
	CEdit*		pMsgEdit = (CEdit*) GetDlgItem(IDC_EDIT_MSG);
	CEdit*		pSrcEdit = (CEdit*) GetDlgItem(IDC_EDIT_SRC);
	CEdit*		pDstEdit = (CEdit*) GetDlgItem(IDC_EDIT_DST);
	CEdit*		pFSButton = (CEdit*) GetDlgItem(IDC_BUTTON_FILESEARCH);
	CEdit*		pFTButton = (CEdit*) GetDlgItem(IDC_BUTTON_FILETRANS);
	CEdit*		pFSEdit = (CEdit*) GetDlgItem(IDC_EDIT_FILESEARCH);

	switch (state)
	{
	case IPC_INITIALIZING:
		pSendButton->EnableWindow(FALSE);
		pMsgEdit->EnableWindow(FALSE);
		m_ListChat.EnableWindow(FALSE);
		pFSButton->EnableWindow(FALSE);
		pFTButton->EnableWindow(FALSE);
		pFSEdit->EnableWindow(FALSE);
		break;
	case IPC_READYTOSEND:
		pSendButton->EnableWindow(TRUE);
		pMsgEdit->EnableWindow(TRUE);
		m_ListChat.EnableWindow(TRUE);
		break;
	case IPC_WAITFORACK:	break;
	case IPC_ERROR:		break;
	case IPC_ADDR_SET:
		pSetAddrButton->SetWindowText("재설정(&R)");
		pSrcEdit->EnableWindow(FALSE);
		pDstEdit->EnableWindow(FALSE);
		pSendButton->EnableWindow(TRUE);
		m_ListChat.EnableWindow(TRUE);
		pMsgEdit->EnableWindow(TRUE);
		pFSButton->EnableWindow(TRUE);
		pFTButton->EnableWindow(TRUE);
		pFSEdit->EnableWindow(TRUE);
		break;
	case IPC_ADDR_RESET:
		pSetAddrButton->SetWindowText("설정(&O)");
		pSrcEdit->EnableWindow(FALSE);
		pDstEdit->EnableWindow(TRUE);
		m_ListChat.EnableWindow(FALSE);
		pSendButton->EnableWindow(FALSE);
		pMsgEdit->EnableWindow(FALSE);
		pFSButton->EnableWindow(FALSE);
		pFTButton->EnableWindow(FALSE);
		pFSEdit->EnableWindow(FALSE);
		m_stDstAdd = "";
		m_stSrcAdd = "";
		m_Ether->SetSourceAddress((unsigned char*)"00000000");
		m_Ether->SetDestinAddress((unsigned char*)"00000000");
		break;
	case IPC_SET_FileOn://button text change
		pFTButton->SetWindowText("취소");
		break;
	case IPC_SET_FileOff://button text change
		pFTButton->SetWindowText("전송");
		break;
	}

	UpdateData(FALSE);
}

void CIPCAppDlg::EndofProcess()
{
	m_LayerMgr.DeAllocLayer();
}


void CIPCAppDlg::OnButtonFileserch() //파일을 찾는 dialog를 띄우고 파일을 선택, 화면에 파일경로명을 출력
{
	CFileDialog fd(TRUE, NULL, NULL, OFN_HIDEREADONLY, "All Files(*.*)|*.*||");

	if (IDOK == fd.DoModal()) { //fd.DoModal을 이용하여 파일선택 dialog를 팝업
		m_strFile = fd.GetPathName(); //파일의 경로명을 받아와서 저장
	}
	UpdateData(FALSE); //변수(m_strFile)에 저장된 값을 화면에 출력
}

void CIPCAppDlg::OnButtonFiletrans()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	if (!m_strFile.IsEmpty())
		if(!FileSended){
		FileSended=TRUE;//file Transfering
		CWinThread* m_FileThread = ::AfxBeginThread(CIPCAppDlg::FileThread, this);
		SetDlgState(IPC_SET_FileOn);

		}else{
		SetDlgState(IPC_SET_FileOff);
		FileSended=FALSE;//init
		}
	else
		AfxMessageBox("Select a file to send first!");

	UpdateData(FALSE);

}

void CIPCAppDlg::OnBnClickedTestPacket()
{
	CString MsgHeader;

	MsgHeader.Format("Send >> ");
	m_stMessage = "Test Packet";
	this->mp_UnderLayer->Send((unsigned char*) (LPCTSTR) m_stMessage, m_stMessage.GetLength(), CHAT_TYPE, CHAT_REC);

}
