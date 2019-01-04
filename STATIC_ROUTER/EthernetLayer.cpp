#include "stdafx.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName ) // 생성시 베이스 레이어 생성자를 호출 
{
	ResetHeader( ) ; // 헤더를 리셋
	memset(&broad, 0xFF, 6);
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
}

ETHERNET_ADDR CEthernetLayer::GetSourceAddress()
{
	return ethsrc; // 자신의 이더넷 주소를 반환
}

void CEthernetLayer::SetSourceAddress(ETHERNET_ADDR pAddress)
{
	ethsrc = pAddress;
	//자신의 주소에 받아온값 복사 
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, LPALL_flag flag)
{
	ppayload = (UCHAR*)(unsigned long)ppayload - (ETH_HSIZE);
	LPETHERNET	m_sHeader = (LPETHERNET)ppayload;//헤더
	BOOL bSuc = false;

	if(m_ARP.getcache()->getARPCache(flag->desip))
		m_sHeader->enet_desaddr = m_ARP.getcache()->getARPCache(flag->desip)->ETHaddr;
	else
		m_sHeader->enet_desaddr = broad;
	m_sHeader->enet_srcaddr = ethsrc;
	// 수신 이더넷 데이터를 상위 레이어에서 받아온 값으로 설정
	m_sHeader->enet_type = htons(flag->app_type); // 상위 프로토콜을 지정
	flag->app_len += ETH_HSIZE;
	// 하위 계층 레이어로 헤더 + 메세지 한 헤더를 보냄
	bSuc = mp_UnderLayer->Send(ppayload, flag); // 하위 레이어에서 받아온 반환값을 리턴
	return bSuc;
}

BOOL CEthernetLayer::Receive( unsigned char* ppayload, LPALL_flag flag)
{
	BOOL bSuccess = false ;
	ppayload = (UCHAR*)(unsigned long)ppayload - (ETH_HSIZE);
	if(mp_UnderLayer->Receive(ppayload, flag))
	{
		LPETHERNET pFrame = (LPETHERNET)ppayload;
		flag->app_type = ntohs(pFrame->enet_type); // 플래그에 상위의 어떤 프로토콜인지 지정
		if((addrcom(pFrame->enet_desaddr, broad) || addrcom(pFrame->enet_desaddr, ethsrc))
			&& !addrcom(pFrame->enet_srcaddr, ethsrc))
			// 브로드 캐스트 거나 나한테 온것을 받아 들이나 내가 보낸것은 걸러냄
		{
			if(flag->app_type == ETHERTYPE_ARP)
			{
				return !m_ARP.getARPpacket(ppayload, flag);
			}
			else if(flag->app_type == ETHERTYPE_IP)
				return true;
		}
	}
	flag->app_type = 0;
	return false;
}

// 맥어드레스 비교
BOOL CEthernetLayer::addrcom(ETHERNET_ADDR adr1, ETHERNET_ADDR adr2) 
{
	for(int i = 0; i < 6; i++)
	{// 루프 돌며 각 인자 검사 
		if(adr1.s_ether_addr[i] != adr2.s_ether_addr[i])
			return false;
	}
	return true;
}
