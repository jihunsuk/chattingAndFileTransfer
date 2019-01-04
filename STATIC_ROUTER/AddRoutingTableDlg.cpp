#include "stdafx.h"
#include "StaticRouter.h"
#include "AddRoutingTableDlg.h"
#include "NILayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CAddRoutingTableDlg::CAddRoutingTableDlg(CWnd* pParent)
	: CDialog(CAddRoutingTableDlg::IDD, pParent){
	m_chk_gate = FALSE;
	m_chk_host = FALSE;
	m_chk_up = true;
}


void CAddRoutingTableDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IP_NETMASK, m_ip_netmask);
	DDX_Control(pDX, IDC_IP_GATE, m_ip_gate);
	DDX_Control(pDX, IDC_IP_DES, m_ip_des);
	DDX_Control(pDX, IDC_COMBO_ETH, m_cbo_eth);
	DDX_Check(pDX, IDC_CHECK_GATE, m_chk_gate);
	DDX_Check(pDX, IDC_CHECK_HOST, m_chk_host);
	DDX_Check(pDX, IDC_CHECK_UP, m_chk_up);
}


BEGIN_MESSAGE_MAP(CAddRoutingTableDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_GATE, OnCheckGate)
	ON_BN_CLICKED(IDC_CHECK_HOST, OnCheckHost)
	END_MESSAGE_MAP()

BOOL CAddRoutingTableDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	int i;
	CString temp;
	ETHERNET_ADDR addr;
	// 네트워크 카드 정보(=MAC ADDR) 값 저장(=String)
	for(i = 0; i < ethcnt; i++){
		addr = ((CNILayer *)(mp_layermng[i].GetLayer("NI")))->getMacAddress();
		temp.Format("%02X:%02X:%02X:%02X:%02X:%02X", addr.s_ether_addr[0], addr.s_ether_addr[1], addr.s_ether_addr[2],
			addr.s_ether_addr[3], addr.s_ether_addr[4], addr.s_ether_addr[5]);
		m_cbo_eth.AddString(temp);
	}
	m_cbo_eth.SetCurSel(0);

	return TRUE;  
}

void CAddRoutingTableDlg::OnOK() {

	UpdateData(true);
	// 빈칸이 존재할 경우
	if(m_ip_des.IsBlank() || m_ip_netmask.IsBlank() || (m_ip_gate.IsBlank() && m_chk_gate)){
		AfxMessageBox("write in the blanks");
		return;
	}
	if(!m_chk_gate && !m_chk_host){
		AfxMessageBox("Select : Gate or Host ");
		return;
	}
	
	// 라우터 테이블의 정보를 저장
	ROUT_TABLE_ITEM item;
	m_ip_des.GetAddress(item.des.s_ip_addr[0], item.des.s_ip_addr[1], item.des.s_ip_addr[2], item.des.s_ip_addr[3]);
	m_ip_netmask.GetAddress(item.netmask.s_ip_addr[0], item.netmask.s_ip_addr[1], item.netmask.s_ip_addr[2], item.netmask.s_ip_addr[3]);

	// 목적주소에 대해 Masking을 통한 저장 수행
	item.des = Andip(item.des, item.netmask);

	// FLAG 변환
	item.flag = 0;
	if(m_chk_up){
		item.flag |= UP_FLAG;
	}

	// GATEWAY
	if(m_chk_gate){
		item.flag |= GATE_FLAG;
		m_ip_gate.GetAddress(item.gateway.s_ip_addr[0], item.gateway.s_ip_addr[1], item.gateway.s_ip_addr[2], item.gateway.s_ip_addr[3]);
	}

	// HOST 
	if(m_chk_host){
		item.flag |= HOST_FLAG;
		memset(&item.gateway.s_ip_addr[0], 0, 4);
	}

	item.metric = 0;
	item.ethcard = m_cbo_eth.GetCurSel();

	// 라우팅 테이블에 데이터 저장
	mp_rtable->addEntry(item);
	CDialog::OnOK();
}

// 부모 매개값 정보 전달
void CAddRoutingTableDlg::setData(CLayerManager *lmng, int cnt, CRoutingTable *rtable){
	mp_layermng = lmng;
	ethcnt = cnt;
	mp_rtable = rtable;
}

void CAddRoutingTableDlg::OnCancel() {
	CDialog::OnCancel();
}

// GATEWAY
void CAddRoutingTableDlg::OnCheckGate() {
	UpdateData(true);
	CWnd *dlg;
	dlg = GetDlgItem(IDC_IP_GATE);
	m_chk_host = false;
	// 게이트 정보 확인
	if(m_chk_gate){
		dlg->EnableWindow(true);
	}
	else{
		dlg->EnableWindow(false);
		m_chk_host = true;
	}
	UpdateData(false);	
}

// HOST
void CAddRoutingTableDlg::OnCheckHost() {
	UpdateData(true);
	CWnd *dlg;
	dlg = GetDlgItem(IDC_IP_GATE);
	m_chk_gate = false;
	if(m_chk_gate){
		dlg->EnableWindow(true);
	}
	else{
		dlg->EnableWindow(false);
		m_chk_host = true;
	}
	UpdateData(false);		
}

// IP주소의 AND 연산
IP_ADDR CAddRoutingTableDlg::Andip(IP_ADDR &ip1, IP_ADDR &ip2){
	IP_ADDR m_andip;
	int i;
	for(i = 0; i < 4; i++){
		m_andip.s_ip_addr[i] = ip1.s_ip_addr[i] & ip2.s_ip_addr[i];
	}
	return m_andip;
}
