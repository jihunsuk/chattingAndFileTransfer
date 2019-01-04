// AddProxyARPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StaticRouter.h"
#include "AddProxyARPDlg.h"
#include "ARPCache.h"
#include "NILayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddProxyARPDlg dialog

//////////////	������	//////////////
CAddProxyARPDlg::CAddProxyARPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddProxyARPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddProxyARPDlg)
	m_str_mac = _T("");
	//}}AFX_DATA_INIT
}

void CAddProxyARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddProxyARPDlg)
	DDX_Control(pDX, IDC_IPADDRESS, m_ip_ip);
	DDX_Control(pDX, IDC_COMBO_ETHCARD, m_cbo_ethcard);
	DDX_Text(pDX, IDC_EDIT_MAC, m_str_mac);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddProxyARPDlg, CDialog)
	//{{AFX_MSG_MAP(CAddProxyARPDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddProxyARPDlg message handlers

void CAddProxyARPDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

////////////// OK ��ư �������� //////////////
void CAddProxyARPDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	// ���� Ȯ��
	if(m_ip_ip.IsBlank() || m_str_mac.IsEmpty())
	{
		AfxMessageBox("��ĭ�� ���� ä�� �ּ���");
		return;
	}
	// ĳ���� ����ϱ� ���� cache ����(��Ʈ���� ����ƽ)
	CARPCache m_cache;

	// DLG �� ������ ������
	PROXY_ARP_ITEM pitem;
	IP_ADDR ipaddr;
	m_ip_ip.GetAddress(ipaddr.s_ip_addr[0], ipaddr.s_ip_addr[1], ipaddr.s_ip_addr[2], ipaddr.s_ip_addr[3]);	
	pitem.IPaddr = ipaddr;
	ETHERNET_ADDR macaddr;
	CharToHex(macaddr.s_ether_addr, m_str_mac);
	pitem.ETHaddr = macaddr;
	pitem.ethcard = m_cbo_ethcard.GetCurSel();

	// ĳ���� ���
	m_cache.setProxy(pitem);
	
	CDialog::OnOK();
}

// ���� dlg �� �θ𿡼� �ʿ��� �Ű����� �ѱ�
void CAddProxyARPDlg::setData(CLayerManager *lmng, int cnt)
{
	mp_layermng = lmng;
	ethcnt = cnt;
}

// ȭ�� �ʱ�ȭ
BOOL CAddProxyARPDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i;
	CString temp;
	ETHERNET_ADDR addr;
	// ��ī�� ������ŭ �ݺ�
	for(i = 0; i < ethcnt; i++)
	{
		// ��ī���� ���� �޺� �ڽ��� ���
		addr = ((CNILayer *)(mp_layermng[i].GetLayer("NI")))->getMacAddress();
		temp.Format("%02X:%02X:%02X:%02X:%02X:%02X", addr.s_ether_addr[0], addr.s_ether_addr[1], addr.s_ether_addr[2],
			addr.s_ether_addr[3], addr.s_ether_addr[4], addr.s_ether_addr[5]);
		m_cbo_ethcard.AddString(temp);		
	}
	m_cbo_ethcard.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ���ڿ��� 16������ �ٲ�
void CAddProxyARPDlg::CharToHex(UCHAR *mac, CString str)
{
 	str.MakeUpper();	// ���� ���ڿ� �빮�ڷ� �ٲ�
 	char* temp = str.LockBuffer();	// char ������ ��ȯ�ϱ� ����
 	UCHAR hex;
 	int i = 0;
 	while(i < 6)	// 6�ڸ� �̹Ƿ�
 	{
 		hex = 0;	
 		if(isdigit(temp[0]))	// �ι�° �ڸ��� ���� ���� 16 ������
 			hex = (temp[0] - '0')*16;
 		else
 			hex = ((temp[0] - 'A') + 10)*16;
 		if(isdigit(temp[1]))	// ù��° �ڸ��� ���� ������
 			hex += (temp[1] - '0');
 		else
 			hex += (temp[1] - 'A') + 10;
 		mac[i++] = hex;		// ���ڸ� 16���� ���� ����
 		temp = temp + 3;	// �����ڸ��� �̵�
 	}
}