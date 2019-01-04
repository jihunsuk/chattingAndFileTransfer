#include "stdafx.h"
#include "NILayer.h"
#include "EthernetLayer.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define OID_802_3_CURRENT_ADDRESS 0x01010102	// 맥어드레스 받는데 필요함 
#define PCAP_OPENFLAG_PROMISCUOUS 1

CNILayer::CNILayer( char* pName ) 
: CBaseLayer( pName )// 생성시 상속받은 베이스 레이어도 함께 생성
{
	eth = NULL;
	if(pcap_findalldevs(&alldevs, errbuf) < 0)	// 이더넷 카드 받아오기
	{
		AfxMessageBox("네트웍 장비를 찾을 수 없습니다.");
		exit(-1);
	}	
}

CNILayer::~CNILayer()
{
	closeETH(); // 열려진 pcap 디스크립터 닫음 
	pcap_freealldevs(alldevs);	// 얻어온 이더넷 정보 메모리 해제 
}

BOOL CNILayer::Send(unsigned char *ppayload, LPALL_flag flag)
{	// pcap_sendpacket을 이용해서 이더넷 카드에 패킷을 보냄
	return !pcap_sendpacket(eth, ppayload, flag->app_len);
}

// pcap_next_ex를 이용해 이더넷 카드에서 한 패킷을 받음 
BOOL CNILayer::Receive(unsigned char *ppayload, LPALL_flag flag)
{	
	const UCHAR *buf;	// 실제 패킷을 받아올 버퍼 next_ex 가 메모리 자동 할당
	struct pcap_pkthdr *header;	//패킷 정보를 담을 헤더 
	
	if((pcap_next_ex(eth, &header, (const UCHAR **)&buf)) > 0)
	{
		flag->ethsel = ethsel;
		memcpy(ppayload, buf, 1514);
		return true;// 이더넷 최대 크기 복사 
	}
	return false;	// 패킷 받아 오는데 실패 
}

// 주어진 이더넷 카드를 연다 
void CNILayer::setETH(int sel)
{
	ethsel = sel;	
	pcap_if_t *trail;	// alldevs 를 순회
	trail = alldevs;	// alldevs 에서 시작
	int i = 0;			// sel 과 순서를 비교하기 위하여 
	while(trail)
	{		
		if((trail->flags != PCAP_IF_LOOPBACK) && (trail->addresses != NULL))
        {// 루프 백이 아니고 주소가 널이 아니면 (생성자에서 받아오는 조건과 같음)
			if(i == sel)	// 선택한 값과 같으면
				break;		// 루프 탈충
			i++;	// 다음 자리로 이동
        }
		trail = trail->next;	// alldevs 의 다음자리로 이동
	}
	eth = pcap_open_live(trail->name, 1514, PCAP_OPENFLAG_PROMISCUOUS, 1, NULL);
	// 버퍼의 크기를 1514, 프로미스 커스 모드 타임아웃을 1ms으로 설정해 어뎁터를염
	
	PACKET_OID_DATA* mac;	// 버퍼와 함수에 넘길 데이터의 포인터
	UCHAR buf[512];	// 실제 mac 어드레스가 저장될 버퍼 
	mac = (PACKET_OID_DATA*)buf;
	mac->Oid = OID_802_3_CURRENT_ADDRESS;
	mac->Length = 6;
	struct sockaddr_in *si;	// ip를 얻어 오기 위해 필요함 소켓 정보 구조체 
	si = (struct sockaddr_in *)trail->addresses->addr;	// 주소정보를 타입 캐스팅함 
	LPADAPTER lpAdapter;	// mac 얻어 오는데 필요함 PacketRequest 함수 용
	lpAdapter = PacketOpenAdapter(trail->name);
	PacketRequest(lpAdapter, FALSE, mac);	// mac 어드레스 가져옴
	memcpy(macad.s_ether_addr, mac->Data, mac->Length);
	memcpy(&ipad, &si->sin_addr.S_un.S_addr, 4);
	PacketCloseAdapter(lpAdapter); // 사용한 어뎁터 닫음 

	((CEthernetLayer *)mp_aUpperLayer[0])->SetSourceAddress(macad);
	((CIPLayer *)mp_aUpperLayer[0]->GetUpperLayer(0))->SetSourceAddress(ipad);
}

ETHERNET_ADDR CNILayer::getMacAddress()
{
	return macad;	// 배열의 맥 어드레스 반환
}

IP_ADDR CNILayer::getIPAddress()
{
	return ipad;	// ip 어드레스 반환
}

void CNILayer::closeETH()
{
	if(eth)	// NULL 이 아니면 (이미 열려 있으면)
	{
		pcap_close(eth);	// 어뎁터 닫음 
		eth = NULL;			// 종료시 확인을 위해 NULL 로 지정
	}
}