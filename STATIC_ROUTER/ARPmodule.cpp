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

// 0xFF 값으로, 6, 4개의 배열인자에 대해 초기화
void CARPmodule::ResetHeader(){
	memset(&ethbroad, 0xFF, 6);
	memset(&ipbroad, 0xFF, 4);
}

// 데이터 전송기능
BOOL CARPmodule::send(unsigned char *ppayload, LPALL_flag flag){
	ARP_CACHE_ITEM* aitem;
	BOOL bsuc = false;

	// 캐시테이블에 IP가 있는지 확인
	aitem = m_cache.getARPCache(flag->desip);

	// IP가 존재하고, 연결된 주소일 경우
	if(aitem && aitem->success){
		flag->app_type = ETHERTYPE_IP;
		// 데이터 송신
		bsuc = ((PETHL)pethlayer)->Send(ppayload, flag);
	}
	else{
		// 목적 주소로 ARP Request를 전송성
		for(int i = 0; i < 3; i++){
			if(!sendARP(flag, ARP_OP_REQUEST))
				return false;
		}

		aitem = m_cache.getARPCache(flag->desip);
		// 성공하지 않은것에 대한 cpu의 할당을 재부여
		for(int i = 0; i< 10 && !aitem->success; i++){
			Sleep(10);
		}

		// reply 도착
		if(aitem->success){
			flag->app_type = ETHERTYPE_IP;
			// 해당 이더넷으로 데이터 전송
			bsuc = ((PETHL)pethlayer)->Send(ppayload, flag);
		}
	}
	return bsuc;
}

// ARP의 Packet Receive
bool CARPmodule::getARPpacket(unsigned char *ppayload, LPALL_flag flag){
	LPARP pFrame = (LPARP)((unsigned long)ppayload + (ETH_HSIZE));

	ARP_CACHE_ITEM aitem;
	PROXY_ARP_ITEM *pitem;

	// 자신에게 온 주소를 검사, 자신에게 보낸 packet의 경우 무시
	if((Comip(pFrame->ip_desaddr, ipbroad) || Comip(pFrame->ip_desaddr, *ipsrc))
		&& !Comip(pFrame->ip_srcaddr, *ipsrc)){		
		// 선택한 이더넷에 대해 설정
		aitem.ethcard = flag->ethsel;
		aitem.success = true;
		aitem.regtime = time(NULL);
		aitem.ETHaddr = pFrame->enet_srcaddr;
		aitem.IPaddr = pFrame->ip_srcaddr;
		// 해당 TTL값을 20분으로 설정
		aitem.remindtime = 20 * 60;
		// 캐시 테이블에 입력
		m_cache.setARPCache(aitem);
		
		flag->app_len = ARP_HSIZE;
		flag->app_type = ETHERTYPE_ARP;

		// Request일 경우
		if(ntohs(pFrame->op) == ARP_OP_REQUEST){
			ALL_flag arpflag;
			arpflag.desip = pFrame->ip_srcaddr;
			arpflag.ethsel = flag->ethsel;

			// Reply의 실패에 따른 Error 메세지 설정
			if(!sendARP(&arpflag, ARP_OP_REPLY)){
				AfxMessageBox("ARP reply 실패");
			}
		}
		return true;
	}
	// 프록시 테이블 저장주소에 대해 ARP 송신
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
			AfxMessageBox("ARP reply 실패");
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

// ARP 전송
BOOL CARPmodule::sendARP(LPALL_flag flag, unsigned short kind, IP_ADDR *srcip, ETHERNET_ADDR *deseth){
	unsigned char buf[28+ETH_HSIZE];
	unsigned char *ppayload = (UCHAR*)(long)buf + ETH_HSIZE;
	LPARP arp_header = (LPARP)ppayload;
	
	// 각 주소에 대해 Host to network 주소타입으로 변경
	arp_header->hard_type	= htons(ARP_HTYPE_ETH);
	arp_header->prot_type	= htons(ETHERTYPE_IP);
	arp_header->hard_size	= ARP_HSIZE_ETH;
	arp_header->prot_size	= ARP_PSIZE_IP;

	// flag 설정
	ALL_flag arpflag = *flag;
	arpflag.app_len = ARP_HSIZE;
	arpflag.app_type = ETHERTYPE_ARP;

	ARP_CACHE_ITEM aitem;

	// Request, gratituous, reply 판별
	switch(kind){
	case ARP_OP_REQUEST:
		aitem.ethcard = flag->ethsel;
		aitem.ETHaddr = ethbroad;
		aitem.IPaddr = flag->desip;
		aitem.success = false;
		aitem.regtime = time(NULL);
		aitem.remindtime = 60 * 3;			
		// 캐시 테이블에 선 등록
		m_cache.setARPCache(aitem);
	case ARP_GRTS:
		arp_header->enet_desaddr = ethbroad;
		arp_header->op = htons(ARP_OP_REQUEST);
		break;
	case ARP_OP_REPLY:
		// 캐시 ARP
		if(m_cache.getARPCache(flag->desip)){
			arp_header->enet_desaddr = m_cache.getARPCache(flag->desip)->ETHaddr;
		}
		// 프록시 ARP
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

// IP 주소 비교
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
