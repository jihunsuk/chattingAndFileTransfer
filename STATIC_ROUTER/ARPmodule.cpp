#include "stdafx.h"
#include "ARPmodule.h"
#include "EthernetLayer.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CARPmodule::CARPmodule(){
	ResetHeader();
}

CARPmodule::~CARPmodule(){

}

// 0xFF ������, 6, 4���� �迭���ڿ� ���� �ʱ�ȭ
void CARPmodule::ResetHeader(){
	memset(&ethbroad, 0xFF, 6);
	memset(&ipbroad, 0xFF, 4);
}

// ������ ���۱��
BOOL CARPmodule::send(unsigned char *ppayload, LPALL_flag flag){
	ARP_CACHE_ITEM* aitem;
	BOOL bsuc = false;

	// ĳ�����̺� IP�� �ִ��� Ȯ��
	aitem = m_cache.getARPCache(flag->desip);

	// IP�� �����ϰ�, ����� �ּ��� ���
	if(aitem && aitem->success){
		flag->app_type = ETHERTYPE_IP;
		// ������ �۽�
		bsuc = ((PETHL)pethlayer)->Send(ppayload, flag);
	}
	else{
		// ���� �ּҷ� ARP Request�� ���ۼ�
		for(int i = 0; i < 3; i++){
			if(!sendARP(flag, ARP_OP_REQUEST))
				return false;
		}

		aitem = m_cache.getARPCache(flag->desip);
		// �������� �����Ϳ� ���� cpu�� �Ҵ��� ��ο�
		for(int i = 0; i< 10 && !aitem->success; i++){
			Sleep(10);
		}

		// reply ����
		if(aitem->success){
			flag->app_type = ETHERTYPE_IP;
			// �ش� �̴������� ������ ����
			bsuc = ((PETHL)pethlayer)->Send(ppayload, flag);
		}
	}
	return bsuc;
}

// ARP�� Packet Receive
bool CARPmodule::getARPpacket(unsigned char *ppayload, LPALL_flag flag){
	LPARP pFrame = (LPARP)((unsigned long)ppayload + (ETH_HSIZE));

	ARP_CACHE_ITEM aitem;
	PROXY_ARP_ITEM *pitem;

	// �ڽſ��� �� �ּҸ� �˻�, �ڽſ��� ���� packet�� ��� ����
	if((Comip(pFrame->ip_desaddr, ipbroad) || Comip(pFrame->ip_desaddr, *ipsrc))
		&& !Comip(pFrame->ip_srcaddr, *ipsrc)){		
		// ������ �̴��ݿ� ���� ����
		aitem.ethcard = flag->ethsel;
		aitem.success = true;
		aitem.regtime = time(NULL);
		aitem.ETHaddr = pFrame->enet_srcaddr;
		aitem.IPaddr = pFrame->ip_srcaddr;
		// �ش� TTL���� 20������ ����
		aitem.remindtime = 20 * 60;
		// ĳ�� ���̺� �Է�
		m_cache.setARPCache(aitem);
		
		flag->app_len = ARP_HSIZE;
		flag->app_type = ETHERTYPE_ARP;

		// Request�� ���
		if(ntohs(pFrame->op) == ARP_OP_REQUEST){
			ALL_flag arpflag;
			arpflag.desip = pFrame->ip_srcaddr;
			arpflag.ethsel = flag->ethsel;

			// Reply�� ���п� ���� Error �޼��� ����
			if(!sendARP(&arpflag, ARP_OP_REPLY)){
				AfxMessageBox("ARP reply ����");
			}
		}
		return true;
	}
	// ���Ͻ� ���̺� �����ּҿ� ���� ARP �۽�
	else if(pitem = m_cache.getProxy(pFrame->ip_desaddr))	{
		aitem.ethcard = flag->ethsel;
		aitem.success = true;
		aitem.regtime = time(NULL);
		aitem.ETHaddr = pFrame->enet_srcaddr;
		aitem.IPaddr = pFrame->ip_srcaddr;
		aitem.remindtime = 20 * 60;
		m_cache.setARPCache(aitem);
		ALL_flag arpflag;
		arpflag.desip = pFrame->ip_srcaddr;
		arpflag.ethsel = flag->ethsel;
		if(!sendARP(&arpflag, ARP_OP_REPLY, &pFrame->ip_desaddr)){
			AfxMessageBox("ARP reply ����");
		}
		return true;
	}
	return false;
}

void CARPmodule::setData(CBaseLayer *pEthLayer){
	pethlayer = pEthLayer;
}

CARPCache* CARPmodule::getcache(){
	return &m_cache;
}

// ARP ����
BOOL CARPmodule::sendARP(LPALL_flag flag, unsigned short kind, IP_ADDR *srcip, ETHERNET_ADDR *deseth){
	unsigned char buf[28+ETH_HSIZE];
	unsigned char *ppayload = (UCHAR*)(long)buf + ETH_HSIZE;
	LPARP arp_header = (LPARP)ppayload;
	
	// �� �ּҿ� ���� Host to network �ּ�Ÿ������ ����
	arp_header->hard_type	= htons(ARP_HTYPE_ETH);
	arp_header->prot_type	= htons(ETHERTYPE_IP);
	arp_header->hard_size	= ARP_HSIZE_ETH;
	arp_header->prot_size	= ARP_PSIZE_IP;

	// flag ����
	ALL_flag arpflag = *flag;
	arpflag.app_len = ARP_HSIZE;
	arpflag.app_type = ETHERTYPE_ARP;

	ARP_CACHE_ITEM aitem;

	// Request, gratituous, reply �Ǻ�
	switch(kind){
	case ARP_OP_REQUEST:
		aitem.ethcard = flag->ethsel;
		aitem.ETHaddr = ethbroad;
		aitem.IPaddr = flag->desip;
		aitem.success = false;
		aitem.regtime = time(NULL);
		aitem.remindtime = 60 * 3;			
		// ĳ�� ���̺� �� ���
		m_cache.setARPCache(aitem);
	case ARP_GRTS:
		arp_header->enet_desaddr = ethbroad;
		arp_header->op = htons(ARP_OP_REQUEST);
		break;
	case ARP_OP_REPLY:
		// ĳ�� ARP
		if(m_cache.getARPCache(flag->desip)){
			arp_header->enet_desaddr = m_cache.getARPCache(flag->desip)->ETHaddr;
		}
		// ���Ͻ� ARP
		else if(deseth){
			aitem.ethcard = flag->ethsel;
			aitem.ETHaddr = *deseth;
			aitem.IPaddr = flag->desip;
			aitem.success = true;
			aitem.regtime = time(NULL);
			aitem.remindtime = 60 * 3;			
			m_cache.setARPCache(aitem);
			arp_header->enet_desaddr = *deseth;
		} 
		else{
			return false;
		}
		arp_header->op = htons(ARP_OP_REPLY);
		break;
	}
	arp_header->ip_desaddr = flag->desip;
	arp_header->enet_srcaddr = ((PETHL)pethlayer)->GetSourceAddress();
	if(srcip)
		arp_header->ip_srcaddr = *srcip;
	else
		arp_header->ip_srcaddr = *ipsrc;

	return ((PETHL)pethlayer)->Send(ppayload, &arpflag);
}

// IP �ּ� ��
BOOL CARPmodule::Comip(IP_ADDR ip1, IP_ADDR ip2)
{
	int i;
	for(i = 0; i < 4; i++){
		if(ip1.s_ip_addr[i] != ip2.s_ip_addr[i])
			return false;
	}
	return true;
}

void CARPmodule::setData(IP_ADDR *pipsrc){
	ipsrc = pipsrc;
}
