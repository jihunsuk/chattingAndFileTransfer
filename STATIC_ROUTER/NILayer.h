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
	// ��Ʈ��ũ ī�� ���� ����
	pcap_if_t *alldevs;
	pcap_t *eth;
	// ���� ����
	char errbuf[PCAP_ERRBUF_SIZE];	
	ETHERNET_ADDR macad;
	IP_ADDR ipad;
	// ������ �̴��� ī�� ��ȣ
	int ethsel;		

public:
	void setETH(int sel);
	void closeETH();					// pcap �� ��ũ���� ���� 
	ETHERNET_ADDR getMacAddress();				// �ƾ�巹���� ȣ���� �ʿ� ��ȯ
	IP_ADDR getIPAddress();
	CNILayer(char* pName);				// ������ 
	virtual ~CNILayer();
	BOOL			Receive(unsigned char *ppayload, LPALL_flag flag) ; // ���� ���̾ �޼��� ���� 
	BOOL			Send( unsigned char* ppayload, LPALL_flag flag); // ���� ���̾�� �޼��� ����
};

#endif // !defined(AFX_NILAYER_H__00FAFA91_5229_4783_9F68_0AC2BAF2B0E7__INCLUDED_)
