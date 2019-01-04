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

// .lib ���� ���� ����
#pragma comment(lib, "Packet.lib")
#pragma comment(lib, "wpcap.lib")

// �� SET ��ư�� ����
#define RTABLERESET 1
#define ARPTABLERESET 2
#define PROXYTABLERESET 3

// Interface UI update
UINT nRegUpdateData;

// Receive Packet ������
void Rcv(TPARAM *param);

// Cache table ������
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
	pcap_findalldevs(&alldevs, errbuf);  // ��Ʈ��ũ ī�� �˻�
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

			// Ni, Ethernet, IP Layer�� ��濬��
			m_LayerMgr[i].ConnectLayers( "NI ( *Ethernet ( *IP ) )" ) ;

			// Setting(=IPLayer) : ARP, ROUTING TABLE
			iplayer->setARP(&ethlayer->m_ARP);
			iplayer->setRoutingTable(&m_routingtable);
			// ARP�� Ethernet Interface card ���� ����
			ethlayer->m_ARP.setData(ethlayer);
			nilayer->setETH(i);
			// Setting(=Receive Thread)
			param[i].dlg = this;
			param[i].iplayer = iplayer;
			param[i].flag1 = &flag1;
			param[i].thrflag = &thrflag[i];
			RecThr[i] = AfxBeginThread((AFX_THREADPROC)Rcv, &param[i]);

			// Thread�� Packet�� �������� Ȯ��
			while(*(param[i].flag1))
				Sleep(100);
			thrflag[i] = true;

			i++;
        }
		trail = trail->next;	// Interface�� ���� �������� ���� ����
	}
	
	ethcnt = i;

	// IPLayes�� �ּҰ��� ���� Ȯ��
	for(i = 0; i < ethcnt; i++)
	{
		int j;
		// Target IPLayer
		CIPLayer* nitarget = (CIPLayer *)(m_LayerMgr[i].GetLayer("IP"));
		for(j = 0; j < ethcnt; j++)
		{
			// Source IPLayer
			CIPLayer* nisource = (CIPLayer *)(m_LayerMgr[j].GetLayer("IP"));
			// Target�� Source�� �ּҰ� ����
			nitarget->setOthIPlayer(nisource);
		}
	}
	
	// ARP���� ������Ʈ
	nRegUpdateData = RegisterWindowMessage( "UpdateData" ) ;
	pcap_freealldevs(alldevs);

	// Cache Thread ����
	CacheThr = AfxBeginThread((AFX_THREADPROC)CacheMng, this);
}

// �Ҹ���
CStaticRouterDlg::~CStaticRouterDlg(){
	isset = false;
	bool flag;
	// Receive Thread ����Ȯ��
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
	// List�� width ����
	int rwidth[] = {100, 100, 100, 50, 45, 49};
	// List�� UI ����
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
	// List�� width ����
	int awidth[] = {90, 90, 36, 45};
	// List�� UI ����
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
	// List�� width ����
	int pwidth[] = {100, 105, 56};
	// List�� UI ����
	for(i = 0; i < 3; i++){
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvcolumn.fmt = LVCFMT_CENTER;
		lvcolumn.pszText = plist[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = pwidth[i];
		m_lst_proxyarp.InsertColumn(i, &lvcolumn);
	}
	m_lst_proxyarp.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// Ethernet Interface Card : ȭ�� ���
	pcap_if_t *alldevs, *trail;
	char errbuf[PCAP_ERRBUF_SIZE];
	CString temp;
	CString nametemp;
	// ��� ������ alldevs�� Linked List ����� ���� ����
	pcap_findalldevs(&alldevs, errbuf);
	trail = alldevs;
	i = 0;
	CNILayer *nilayer;
	IP_ADDR ipaddr;
	ETHERNET_ADDR ethaddr;

	// NiLayer���� �� ��Ʈ��ũ ����(=IP, MAC)�� �޾ƿ� �� ȭ�鿡 ���
	while(trail){		
		if((trail->flags != PCAP_IF_LOOPBACK) && (trail->addresses != NULL)){
			// NI ���̾�� ��Ʈ�� ī�� IP �� MAC �ּ� �޾ƿ�
			nilayer = (CNILayer *)m_LayerMgr[i].GetLayer("NI");
			ipaddr = nilayer->getIPAddress();
			ethaddr = nilayer->getMacAddress();
			nametemp = trail->description;
			nametemp = nametemp.Left(40);
			// String ���·� �� ������ ����
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

// Update�� ���� Message ����
LRESULT CStaticRouterDlg::OnRegUpdatedata(WPARAM wParam, LPARAM lParam){
	// ARP���� ����
	resetList(ARPTABLERESET);
	return 0;
}

// List�� ���� Reset /
void CStaticRouterDlg::resetList(int flag){
	int i;
	// ����� ���̺�
	ROUT_TABLE_ITEM ritem;
	// ���Ͻ� ���̺�
	PROXY_ARP_ITEM pitem;
	// ĳ�� ���̺�
	ARP_CACHE_ITEM aitem;
	LV_ITEM lvItem;
	CString temp;
	CARPCache m_cache;
	UpdateData(true);
	
	int psel = m_lst_proxyarp.GetSelectionMark();
	int asel = m_lst_arpcache.GetSelectionMark();
	int rsel = m_lst_routing.GetSelectionMark();
	// switch mode�� ���� ���� �̵� ����
	switch(flag){
	// ����� ���̺�
	case RTABLERESET:
		m_lst_routing.DeleteAllItems();
		// ����� ���̺��� ������ ���� ��ŭ ����
		for(i = 0; i < m_routingtable.getEntryCnt(); i++){
			ritem = *(m_routingtable.getEntry(i));
			int j=0;
			int nItemNum = m_lst_routing.GetItemCount();
			
			// String ���� ���� : ���� �ּ�
			temp.Format("%d.%d.%d.%d", ritem.des.s_ip_addr[0], ritem.des.s_ip_addr[1], ritem.des.s_ip_addr[2], ritem.des.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.InsertItem(&lvItem);

			// String ���� ���� : ����ŷ ����
			temp.Format("%d.%d.%d.%d", ritem.netmask.s_ip_addr[0], ritem.netmask.s_ip_addr[1], ritem.netmask.s_ip_addr[2], ritem.netmask.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);

			// String ���� ���� : ����Ʈ���� ����
			temp.Format("%d.%d.%d.%d", ritem.gateway.s_ip_addr[0], ritem.gateway.s_ip_addr[1], ritem.gateway.s_ip_addr[2], ritem.gateway.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);

			// String ���� ���� : FLAG ���� ����
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
			
			// String ���� ���� : �̴��� ����
			temp.Format("%d", ritem.ethcard);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);

			// String ���� ���� : ��Ʈ�� ���� 
			temp.Format("%d", ritem.metric);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_routing.SetItem(&lvItem);
		}
		break;

	// ĳ�� ���̺�
	case ARPTABLERESET:
		m_lst_arpcache.DeleteAllItems();

		// ĳ�� ���̺� ������ ��ŭ ���� ����
		for(i = 0; i < m_cache.getARPcnt(); i++){
			aitem = *m_cache.getARPCache(i);
			int j = 0;
			int nItemNum = m_lst_arpcache.GetItemCount();
			
			// String ���� ���� : SourceIP �ּ�
			temp.Format("%d.%d.%d.%d", aitem.IPaddr.s_ip_addr[0], aitem.IPaddr.s_ip_addr[1], aitem.IPaddr.s_ip_addr[2], aitem.IPaddr.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_arpcache.InsertItem(&lvItem);

			// String ���� ���� : �̴��� �ּ�
			temp.Format("%02X:%02X:%02X:%02X:%02X:%02X", aitem.ETHaddr.s_ether_addr[0], aitem.ETHaddr.s_ether_addr[1], aitem.ETHaddr.s_ether_addr[2]
				, aitem.ETHaddr.s_ether_addr[3], aitem.ETHaddr.s_ether_addr[4], aitem.ETHaddr.s_ether_addr[5]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_arpcache.SetItem(&lvItem);
			
			// String ���� ���� : ��Ʈ��ũ ī�� ����
			temp.Format("%d", aitem.ethcard);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_arpcache.SetItem(&lvItem);
	
			// String ���� ���� : �������� ����(=TTL ����)
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
		// ���Ͻ� ���̺� ������ ������ŭ ���� ����
		for(i = 0; i < m_cache.getProxycnt(); i++){
			pitem = *m_cache.getProxy(i);

			int j=0;
			int nItemNum = m_lst_proxyarp.GetItemCount();

			// String ���� ���� : SourceIP ����
			temp.Format("%d.%d.%d.%d", pitem.IPaddr.s_ip_addr[0], pitem.IPaddr.s_ip_addr[1], pitem.IPaddr.s_ip_addr[2], pitem.IPaddr.s_ip_addr[3]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_proxyarp.InsertItem(&lvItem);

			// String ���� ���� : �̴��� �ּ� 
			temp.Format("%02X:%02X:%02X:%02X:%02X:%02X", pitem.ETHaddr.s_ether_addr[0], pitem.ETHaddr.s_ether_addr[1], pitem.ETHaddr.s_ether_addr[2]
				, pitem.ETHaddr.s_ether_addr[3], pitem.ETHaddr.s_ether_addr[4], pitem.ETHaddr.s_ether_addr[5]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_proxyarp.SetItem(&lvItem);

			// String ���� ���� : ��Ʈ��ũ ����/
			temp.Format("%d", pitem.ethcard);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = nItemNum;
			lvItem.iSubItem = j++;
			lvItem.pszText = (LPTSTR)(LPCSTR)temp;
			m_lst_proxyarp.SetItem(&lvItem);
		}
		break;
	}

	// ������ ��ü�� ���� Focus�� �������� �ְ� ����
	m_lst_routing.SetSelectionMark(rsel);
	m_lst_routing.SetItemState(rsel, LVIS_SELECTED, LVIS_SELECTED);
	m_lst_arpcache.SetSelectionMark(asel);
	m_lst_arpcache.SetItemState(asel, LVIS_SELECTED, LVIS_SELECTED);
	m_lst_proxyarp.SetSelectionMark(psel);
	m_lst_proxyarp.SetItemState(psel, LVIS_SELECTED, LVIS_SELECTED);
	
	UpdateData(false);
}

// ����� ���̺� �߰�
void CStaticRouterDlg::OnButtonSradd() {
	CAddRoutingTableDlg dlg;
	dlg.setData(m_LayerMgr, ethcnt, &m_routingtable);
	if(dlg.DoModal() == IDOK)
		resetList(RTABLERESET);
}

// ����� ���̺� ����
void CStaticRouterDlg::OnButtonSrdel() {
	UpdateData(true);
	int sel = m_lst_routing.GetSelectionMark();
	if(sel > -1){
		m_routingtable.delEntry(sel);
		resetList(RTABLERESET);
	}
}

// ARP ���̺� ����
void CStaticRouterDlg::OnButtonArpdel() {
	UpdateData(true);
	int sel = m_lst_arpcache.GetSelectionMark();
	if(sel > -1){
		CARPCache m_cache;
		m_cache.delARPcache(sel);
		resetList(ARPTABLERESET);
	}	
}

// ���Ͻ� ARP �߰�
void CStaticRouterDlg::OnButtonProxyadd() {
	CAddProxyARPDlg dlg;
	dlg.setData(m_LayerMgr, ethcnt);
	if(dlg.DoModal()==IDOK)
		resetList(PROXYTABLERESET);
}

// ���Ͻ� ARP ����
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

// ĳ�� �޴���
void CacheMng(CStaticRouterDlg *dlg)
{
	CARPCache m_cache;
	time_t crtime;	// ���� �ð�
	ARP_CACHE_ITEM* item;
	time_t m20, m3;
	// ���� : 20��, ���� : 3��
	m20 = 60 * 20;
	m3 = 60 * 3;
	bool flag;

	while(dlg->isset){
		flag = false;
		for(int i = 0; i < m_cache.getARPcnt(); i++)
		{
			crtime = time(NULL);
			item = m_cache.getARPCache(i);
			 // ���� 20��
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
			// ���� 3��
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
		// �����Ǿ��� ��� DLG �� �޼��� ����
		if(flag)
			::PostMessage(HWND_BROADCAST, nRegUpdateData, NULL, NULL);
		// CPU�� �Ҵ緮�� �ٸ� �����忡�� �ο�
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
	// ������ ũ�� ����
	ppayload = (UCHAR*)((long)buf + ETH_HSIZE + IP_HSIZE);
	while(tparam.dlg->isset){
		// Receive ���� ���� �Ǵ�
		if(!tparam.iplayer->Receive(ppayload, &rcvflag)){
			// ARP ��� Ȯ��
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
