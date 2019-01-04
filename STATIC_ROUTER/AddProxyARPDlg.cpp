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

//////////////	생성자	//////////////
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

////////////// OK 버튼 눌렀을때 //////////////
void CAddProxyARPDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	// 조건 확인
	if(m_ip_ip.IsBlank() || m_str_mac.IsEmpty())
	{
		AfxMessageBox("빈칸을 전부 채워 주세요");
		return;
	}
	// 캐쉬에 등록하기 위해 cache 생성(엔트리가 스태틱)
	CARPCache m_cache;

	// DLG 의 정보를 가져옴
	PROXY_ARP_ITEM pitem;
	IP_ADDR ipaddr;
	m_ip_ip.GetAddress(ipaddr.s_ip_addr[0], ipaddr.s_ip_addr[1], ipaddr.s_ip_addr[2], ipaddr.s_ip_addr[3]);	
	pitem.IPaddr = ipaddr;
	ETHERNET_ADDR macaddr;
	CharToHex(macaddr.s_ether_addr, m_str_mac);
	pitem.ETHaddr = macaddr;
	pitem.ethcard = m_cbo_ethcard.GetCurSel();

	// 캐쉬에 등록
	m_cache.setProxy(pitem);
	
	CDialog::OnOK();
}

// 현재 dlg 에 부모에서 필요한 매개변수 넘김
void CAddProxyARPDlg::setData(CLayerManager *lmng, int cnt)
{
	mp_layermng = lmng;
	ethcnt = cnt;
}

// 화면 초기화
BOOL CAddProxyARPDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i;
	CString temp;
	ETHERNET_ADDR addr;
	// 랜카드 개수만큼 반복
	for(i = 0; i < ethcnt; i++)
	{
		// 랜카드의 정보 콤보 박스에 등록
		addr = ((CNILayer *)(mp_layermng[i].GetLayer("NI")))->getMacAddress();
		temp.Format("%02X:%02X:%02X:%02X:%02X:%02X", addr.s_ether_addr[0], addr.s_ether_addr[1], addr.s_ether_addr[2],
			addr.s_ether_addr[3], addr.s_ether_addr[4], addr.s_ether_addr[5]);
		m_cbo_ethcard.AddString(temp);		
	}
	m_cbo_ethcard.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// 문자열을 16진수로 바꿈
void CAddProxyARPDlg::CharToHex(UCHAR *mac, CString str)
{
 	str.MakeUpper();	// 얻어온 문자열 대문자로 바꿈
 	char* temp = str.LockBuffer();	// char 형으로 변환하기 위함
 	UCHAR hex;
 	int i = 0;
 	while(i < 6)	// 6자리 이므로
 	{
 		hex = 0;	
 		if(isdigit(temp[0]))	// 두번째 자리는 값을 구해 16 곱해줌
 			hex = (temp[0] - '0')*16;
 		else
 			hex = ((temp[0] - 'A') + 10)*16;
 		if(isdigit(temp[1]))	// 첫번째 자리는 값을 구해줌
 			hex += (temp[1] - '0');
 		else
 			hex += (temp[1] - 'A') + 10;
 		mac[i++] = hex;		// 두자리 16진수 합을 저장
 		temp = temp + 3;	// 다음자리로 이동
 	}
}