#include "stdafx.h"
#include "StaticRouter.h"
#include "StaticRouterDlg.h"
#include "AddRoutingTableDlg.h"
#include "AddProxyARPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "NILayer.h"
#include "EthernetLayer.h"
#include "IPLayer.h"

#include <pcap.h>
#include <Packet32.h>

// .lib 파일 강제 추출
#pragma comment(lib, "Packet.lib")
#pragma comment(lib, "wpcap.lib")

// 각 SET 버튼값 정의
#define RTABLERESET 1
#define ARPTABLERESET 2
#define PROXYTABLERESET 3

// Interface UI update
UINT nRegUpdateData;

// Receive Packet 쓰레드
void Rcv(TPARAM *param);

// Cache table 쓰레드
void CacheMng(CStaticRouterDlg *dlg);

class CAboutDlg : public CDialog{
public:
	CAboutDlg();
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD){
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CStaticRouterDlg::CStaticRouterDlg(CWnd* pParent)
: CDialog(CStaticRouterDlg::IDD, pParent){

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pcap_if_t *alldevs, *trail;	//  List of Ethernet Interface Card
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_findalldevs(&alldevs, errbuf);  // 네트워크 카드 검색
	trail = alldevs;
	int i = 0;			
	isset = true;
	
	while(trail)
	{		
		if((trail->flags != PCAP_IF_LOOPBACK) && (trail->addresses != NULL)){
			CEthernetLayer *ethlayer;
			CIPLayer *iplayer;
			CNILayer *nilayer;
			TPARAM param[MAX_ETHERNET_CARD];
			bool flag1 = true;
			
			m_LayerMgr[i].AddLayer( iplayer = new CIPLayer( "IP" ) );
			m_LayerMgr[i].AddLayer( ethlayer = new CEthernetLayer( "Ethernet" ) );
			m_LayerMgr[i].AddLayer( nilayer = new CNILayer( "NI" ) );

			// Ni, Ethernet, IP Layer의 양방연결
			m_LayerMgr[i].ConnectLayers( "NI ( *Ethernet ( *IP ) )" ) ;

			// Setting(=IPLayer) : ARP, ROUTING TABLE
			iplayer->setARP(&ethlayer->m_ARP);
			iplayer->setRoutingTable(&m_routingtable);
			// ARP에 Ethernet Interface card 정보 삽입
			ethlayer->m_ARP.setData(ethlayer);
			nilayer->setETH(i);
			// Setting(=Receive Thread)
			param[i].dlg = this;
			param[i].iplayer = iplayer;
			param[i].flag1 = &flag1;
			param[i].thrflag = &thrflag[i];
			RecThr[i] = AfxBeginThread((AFX_THREADPROC)Rcv, &param[i]);

			// Thread가 Packet을 가졌는지 확인
			while(*(param[i].flag1))
				Sleep(100);
			thrflag[i] = true;

			i++;
        }
		trail = trail->next;	// Interface에 대한 지속적인 정의 수행
	}
	
	ethcnt = i;

	// IPLayes간 주소값을 서로 확인
	for(i = 0; i < ethcnt; i++)
	{
		int j;
		// Target IPLayer
		CIPLayer* nitarget = (CIPLayer *)(m_LayerMgr[i].GetLayer("IP"));
		for(j = 0; j < ethcnt; j++)
		{
			// Source IPLayer
			CIPLayer* nisource = (CIPLayer *)(m_LayerMgr[j].GetLayer("IP"));
			// Target에 Source의 주소값 저장
			nitarget->setOthIPlayer(nisource);
		}
	}
	
	// ARP정보 업데이트
	nRegUpdateData = RegisterWindowMessage( "UpdateData" ) ;
	pcap_freealldevs(alldevs);

	// Cache Thread 수행
	CacheThr = AfxBeginThread((AFX_THREADPROC)CacheMng, this);
}

// 소멸자
CStaticRouterDlg::~CStaticRouterDlg(){
	isset = false;
	bool flag;
	// Receive Thread 종료확인
	do{
		flag = false;
		for(int i = 0; i < ethcnt; i++){
			if(thrflag[i] == true){
				flag = true;				
			}
		}
	}while(flag);

	// Deallocation Layers
	for(int i = 0; i < ethcnt; i++){
		m_LayerMgr[i].DeAllocLayer();
	}
}

void CStaticRouterDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ETHCARD, m_lst_ethcard);
	DDX_Control(pDX, IDC_LIST_ROUTING_TABLE, m_lst_routing);
	DDX_Control(pDX, IDC_LIST_PROXYARP, m_lst_proxyarp);
	DDX_Control(pDX, IDC_LIST_ARPCACHE, m_lst_arpcache);
}

BEGIN_MESSAGE_MAP(CStaticRouterDlg, CDialog)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(ID_BUTTON_SRADD, OnButtonSradd)
ON_BN_CLICKED(IDC_BUTTON_SRDEL, OnButtonSrdel)
ON_BN_CLICKED(ID_BUTTON_PROXYADD, OnButtonProxyadd)
ON_BN_CLICKED(ID_BUTTON_ARPDEL, OnButtonArpdel)
ON_BN_CLICKED(ID_BUTTON_PROXYDEL, OnButtonProxydel)
ON_REGISTERED_MESSAGE(nRegUpdateData, OnRegUpdatedata)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROUTING_TABLE, &CStaticRouterDlg::OnLvnItemchangedListRoutingTable)
END_MESSAGE_MAP()

BOOL CStaticRouterDlg::OnInitDialog(){
	CDialog::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL){
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty()){
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	
	int i;
	LV_COLUMN lvcolumn;
	
	// List of Routing Table
	char rlist[][20] = {"Destination", "NetMask", "Gateway", "Flag", "Interface", "Metric"};
	// List의 width 정의
	int rwidth[] = {100, 100, 100, 50, 45, 49};
	// List의 UI 정의
	for(i = 0; i < 6; i++){
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_CENTER;
		lvcolumn.pszText = rlist[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = rwidth[i];
		m_lst_routing.InsertColumn(i, &lvcolumn);
	}

	m_lst_routing.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// List of Cache Table
	char alist[][20] = {"IP Address","Ethernet Address","Interface","Flag"};
	// List의 width 정의
	int awidth[] = {90, 90, 36, 45};
	// List의 UI 정의
	for(i = 0; i < 4; i++){
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_CENTER;
		lvcolumn.pszText = alist[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = awidth[i];
		m_lst_arpcache.InsertColumn(i, &lvcolumn);
	}
	m_lst_arpcache.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	
	// List of Proxy Table
	char plist[][20] = {"IP Address","Ethernet Address","Interface"};
	// List의 width 정의
	int pwidth[] = {100, 105, 56};
	// List의 UI 정의
	for(i = 0; i < 3; i++){
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_CENTER;
		lvcolumn.pszText = plist[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = pwidth[i];
		m_lst_proxyarp.InsertColumn(i, &lvcolumn);
	}
	m_lst_proxyarp.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// Ethernet Interface Card : 화면 출력
	pcap_if_t *alldevs, *trail;
	char errbuf[PCAP_ERRBUF_SIZE];
	CString temp;
	CString nametemp;
	// 모든 정보를 alldevs의 Linked List 기법에 의해 연결
	pcap_findalldevs(&alldevs, errbuf);
	trail = alldevs;
	i = 0;
	CNILayer *nilayer;
	IP_ADDR ipaddr;
	ETHERNET_ADDR ethaddr;

	// NiLayer에서 각 네트워크 정보(=IP, MAC)을 받아온 후 화면에 출력
	while(trail){		
		if((trail->flags != PCAP_IF_LOOPBACK) && (trail->addresses != NULL)){
			// NI 레이어에서 네트웍 카드 IP 와 MAC 주소 받아옴
			nilayer = (CNILayer *)m_LayerMgr[i].GetLayer("NI");
			ipaddr = nilayer->getIPAddress();
			ethaddr = nilayer->getMacAddress();
			nametemp = trail->description;
			nametemp = nametemp.Left(40);
			// String 형태로 각 데이터 저장
			temp.Format("%d, %s : %d.%d.%d.%d, %02X:%02X:%02X:%02X:%02X:%02X", i, nametemp,
				ipaddr.s_ip_addr[0], ipaddr.s_ip_addr[1], ipaddr.s_ip_addr[2], ipaddr.s_ip_addr[3], 
				ethaddr.s_ether_addr[0], ethaddr.s_ether_addr[1], ethaddr.s_ether_addr[2], ethaddr.s_ether_addr[3], ethaddr.s_ether_addr[4], ethaddr.s_ether_addr[5]);
			m_lst_ethcard.AddString(temp);
			i++;
		}
		trail = trail->next;
	}	
	pcap_freealldevs(alldevs);

	return TRUE;
}

void CStaticRouterDlg::OnSysCommand(UINT nID, LPARAM lParam){
	if ((nID & 0xFFF0) == IDM_ABOUTBOX){
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CStaticRouterDlg::OnPaint() 
{
	if (IsIconic()){
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else{
		CDialog::OnPaint();
	}
}

HCURSOR CStaticRouterDlg::OnQueryDragIcon(){
	return (HCURSOR) m_hIcon;
}

// Update에 대한 Message 수신
LRESULT CStaticRouterDlg::OnRegUpdatedata(WPARAM wParam, LPARAM lParam){
	// ARP정보 갱신
	resetList(ARPTABLERESET);
	return 0;
}

// List에 대한 Reset /
void CStaticRouterDlg::resetList(int flag){
	int i;
	// 라우팅 테이블
	ROUT_TABLE_ITEM ritem;
	// 프록시 테이블
	PROXY_ARP_ITEM pitem;
	// 캐시 테이블
	ARP_CACHE_ITEM aitem;
	LV_ITEM lvItem;
	CString temp;
	CARPCache m_cache;
	UpdateData(true);
	
	int psel = m_lst_proxyarp.GetSelectionMark();
	int asel = m_lst_arpcache.GetSelectionMark();
	int rsel = m_lst_routing.GetSelectionMark();
	// switch mode에 의한 직접 이동 수행
	switch(flag){
	// 라우터 테이블
	case RTABLERESET:
		m_lst_routing.DeleteAllItems();
		// 라우터 테이블의 데이터 갯수 만큼 수행
		for(i = 0; i < m_routingtable.getEntryCnt(); i++){
			ritem = *(m_routingtable.getEntry(i));
			int j=0;
			int nItemNum = m_lst_routing.GetItemCount();
			
			// String 형태 저장 : 목적 주소
			temp.Format("%d.%d.%d.%d", ritem.des.s_ip_addr[0], ritem.des.s_ip_addr[1], ritem.des.s_ip_addr[2], ritem.des.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.InsertItem(&lvItem);

			// String 형태 저장 : 마스킹 정보
			temp.Format("%d.%d.%d.%d", ritem.netmask.s_ip_addr[0], ritem.netmask.s_ip_addr[1], ritem.netmask.s_ip_addr[2], ritem.netmask.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);

			// String 형태 저장 : 게이트웨이 정보
			temp.Format("%d.%d.%d.%d", ritem.gateway.s_ip_addr[0], ritem.gateway.s_ip_addr[1], ritem.gateway.s_ip_addr[2], ritem.gateway.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);

			// String 형태 저장 : FLAG 정보 저장
			temp.Empty();
			if(ritem.flag & UP_FLAG)
				temp += "U";
			if(ritem.flag & GATE_FLAG)
				temp += "G";
			if(ritem.flag & HOST_FLAG)
				temp += "H";
			
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);
			
			// String 형태 저장 : 이더넷 정보
			temp.Format("%d", ritem.ethcard);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);

			// String 형태 저장 : 메트릭 정보 
			temp.Format("%d", ritem.metric);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);
		}
		break;

	// 캐시 테이블
	case ARPTABLERESET:
		m_lst_arpcache.DeleteAllItems();

		// 캐시 테이블 데이터 만큼 루프 수행
		for(i = 0; i < m_cache.getARPcnt(); i++){
			aitem = *m_cache.getARPCache(i);
			int j = 0;
			int nItemNum = m_lst_arpcache.GetItemCount();
			
			// String 형태 저장 : SourceIP 주소
			temp.Format("%d.%d.%d.%d", aitem.IPaddr.s_ip_addr[0], aitem.IPaddr.s_ip_addr[1], aitem.IPaddr.s_ip_addr[2], aitem.IPaddr.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_arpcache.InsertItem(&lvItem);

			// String 형태 저장 : 이더넷 주소
			temp.Format("%02X:%02X:%02X:%02X:%02X:%02X", aitem.ETHaddr.s_ether_addr[0], aitem.ETHaddr.s_ether_addr[1], aitem.ETHaddr.s_ether_addr[2]
				, aitem.ETHaddr.s_ether_addr[3], aitem.ETHaddr.s_ether_addr[4], aitem.ETHaddr.s_ether_addr[5]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_arpcache.SetItem(&lvItem);
			
			// String 형태 저장 : 네트워크 카드 정보
			temp.Format("%d", aitem.ethcard);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_arpcache.SetItem(&lvItem);
	
			// String 형태 저장 : 성공여부 저장(=TTL 여부)
			temp.Format("%s, %ds", aitem.success?"C":"I", aitem.remindtime);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_arpcache.SetItem(&lvItem);
		}
		break;
	// Proxy Table
	case PROXYTABLERESET:
		m_lst_proxyarp.DeleteAllItems();
		// 프록시 테이블내 데이터 갯수만큼 루프 수행
		for(i = 0; i < m_cache.getProxycnt(); i++){
			pitem = *m_cache.getProxy(i);

			int j=0;
			int nItemNum = m_lst_proxyarp.GetItemCount();

			// String 형태 저장 : SourceIP 정보
			temp.Format("%d.%d.%d.%d", pitem.IPaddr.s_ip_addr[0], pitem.IPaddr.s_ip_addr[1], pitem.IPaddr.s_ip_addr[2], pitem.IPaddr.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_proxyarp.InsertItem(&lvItem);

			// String 형태 저장 : 이더넷 주소 
			temp.Format("%02X:%02X:%02X:%02X:%02X:%02X", pitem.ETHaddr.s_ether_addr[0], pitem.ETHaddr.s_ether_addr[1], pitem.ETHaddr.s_ether_addr[2]
				, pitem.ETHaddr.s_ether_addr[3], pitem.ETHaddr.s_ether_addr[4], pitem.ETHaddr.s_ether_addr[5]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_proxyarp.SetItem(&lvItem);

			// String 형태 저장 : 네트워크 정보/
			temp.Format("%d", pitem.ethcard);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_proxyarp.SetItem(&lvItem);
		}
		break;
	}

	// 선택한 객체에 대해 Focus가 맞춰질수 있게 수행
	m_lst_routing.SetSelectionMark(rsel);
	m_lst_routing.SetItemState(rsel, LVIS_SELECTED, LVIS_SELECTED);
	m_lst_arpcache.SetSelectionMark(asel);
	m_lst_arpcache.SetItemState(asel, LVIS_SELECTED, LVIS_SELECTED);
	m_lst_proxyarp.SetSelectionMark(psel);
	m_lst_proxyarp.SetItemState(psel, LVIS_SELECTED, LVIS_SELECTED);
	
	UpdateData(false);
}

// 라우팅 테이블 추가
void CStaticRouterDlg::OnButtonSradd() {
	CAddRoutingTableDlg dlg;
	dlg.setData(m_LayerMgr, ethcnt, &m_routingtable);
	if(dlg.DoModal() == IDOK)
		resetList(RTABLERESET);
}

// 라우팅 테이블 삭제
void CStaticRouterDlg::OnButtonSrdel() {
	UpdateData(true);
	int sel = m_lst_routing.GetSelectionMark();
	if(sel > -1){
		m_routingtable.delEntry(sel);
		resetList(RTABLERESET);
	}
}

// ARP 테이블 삭제
void CStaticRouterDlg::OnButtonArpdel() {
	UpdateData(true);
	int sel = m_lst_arpcache.GetSelectionMark();
	if(sel > -1){
		CARPCache m_cache;
		m_cache.delARPcache(sel);
		resetList(ARPTABLERESET);
	}	
}

// 프록시 ARP 추가
void CStaticRouterDlg::OnButtonProxyadd() {
	CAddProxyARPDlg dlg;
	dlg.setData(m_LayerMgr, ethcnt);
	if(dlg.DoModal()==IDOK)
		resetList(PROXYTABLERESET);
}

// 프록시 ARP 삭제
void CStaticRouterDlg::OnButtonProxydel() {
	UpdateData(true);
	int sel = m_lst_proxyarp.GetSelectionMark();
	if(sel > -1)
	{
		CARPCache m_cache;
		m_cache.delProxycache(sel);
		resetList(PROXYTABLERESET);
	}		
}

// 캐쉬 메니저
void CacheMng(CStaticRouterDlg *dlg)
{
	CARPCache m_cache;
	time_t crtime;	// 현재 시간
	ARP_CACHE_ITEM* item;
	time_t m20, m3;
	// 성공 : 20분, 실패 : 3분
	m20 = 60 * 20;
	m3 = 60 * 3;
	bool flag;

	while(dlg->isset){
		flag = false;
		for(int i = 0; i < m_cache.getARPcnt(); i++)
		{
			crtime = time(NULL);
			item = m_cache.getARPCache(i);
			 // 성공 20분
			if(item->success)
			{
				item->remindtime = item->regtime + m20 - crtime;
				if(item->remindtime <= 0)
				{
					m_cache.delARPcache(i);
					i--;
					flag = true;
				}
			}
			// 실패 3분
			else
			{
				item->remindtime = item->regtime + m3 - crtime;
				if(item->remindtime <= 0)
				{
					m_cache.delARPcache(i);
					i--;
					flag = true;
				}
			}
		}
		// 수정되었을 경우 DLG 로 메세지 전송
		if(flag)
			::PostMessage(HWND_BROADCAST, nRegUpdateData, NULL, NULL);
		// CPU의 할당량을 다른 쓰레드에게 부여
		Sleep(1000);
	}
	return;
}

// Receive Thread
void Rcv(TPARAM *param)
{
	TPARAM tparam = *param;
	*(param->flag1) = false;
	
	ALL_flag rcvflag;
	// Message 
	UCHAR* buf[1514];
	
	UCHAR* ppayload;
	// 데이터 크기 지정
	ppayload = (UCHAR*)((long)buf + ETH_HSIZE + IP_HSIZE);
	while(tparam.dlg->isset){
		// Receive 수행 여부 판단
		if(!tparam.iplayer->Receive(ppayload, &rcvflag)){
			// ARP 기능 확인
			if(rcvflag.app_type)
				::PostMessage(HWND_BROADCAST, nRegUpdateData, NULL, NULL);
		}
	}
	*tparam.thrflag = false;
	return;
}


void CStaticRouterDlg::OnLvnItemchangedListRoutingTable(NMHDR *pNMHDR, LRESULT *pResult){
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
}
