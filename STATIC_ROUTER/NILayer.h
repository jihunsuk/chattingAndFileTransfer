// NILayer.h: interface for the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NILAYER_H__00FAFA91_5229_4783_9F68_0AC2BAF2B0E7__INCLUDED_)
#define AFX_NILAYER_H__00FAFA91_5229_4783_9F68_0AC2BAF2B0E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <pcap.h>
#include <Packet32.h>
#pragma comment(lib, "Packet.lib")
#pragma comment(lib, "wpcap.lib")

#include "BaseLayer.h"

#define MAX_PACKET_SIZE 1514

class CNILayer : public CBaseLayer  
{
private:
	// 네트워크 카드 저장 변수
	pcap_if_t *alldevs;
	pcap_t *eth;
	// 버퍼 저장
	char errbuf[PCAP_ERRBUF_SIZE];	
	ETHERNET_ADDR macad;
	IP_ADDR ipad;
	// 선택한 이더넷 카드 번호
	int ethsel;		

public:
	void setETH(int sel);
	void closeETH();					// pcap 용 디스크립터 해제 
	ETHERNET_ADDR getMacAddress();				// 맥어드레스를 호출한 쪽에 반환
	IP_ADDR getIPAddress();
	CNILayer(char* pName);				// 생성자 
	virtual ~CNILayer();
	BOOL			Receive(unsigned char *ppayload, LPALL_flag flag) ; // 상위 레이어에 메세지 보냄 
	BOOL			Send( unsigned char* ppayload, LPALL_flag flag); // 하위 레이어로 메세지 보냄
};

#endif // !defined(AFX_NILAYER_H__00FAFA91_5229_4783_9F68_0AC2BAF2B0E7__INCLUDED_)
