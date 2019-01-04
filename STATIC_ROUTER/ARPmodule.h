#if !defined(AFX_ARPMODULE_H__40403A17_6976_4B7D_AC8A_7AEF5B7AD039__INCLUDED_)
#define AFX_ARPMODULE_H__40403A17_6976_4B7D_AC8A_7AEF5B7AD039__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "BaseLayer.h"
#include "ARPCache.h"

#define PIPL CIPLayer *
#define PETHL CEthernetLayer *

class CARPmodule
{	
public:
	void setData(IP_ADDR* pipsrc);
	void setData(CBaseLayer *pEthLayer);
	BOOL sendARP(LPALL_flag flag, unsigned short kind, IP_ADDR *srcip = NULL, ETHERNET_ADDR *deseth = NULL);
	bool getARPpacket(unsigned char *ppayload, LPALL_flag flag);
	BOOL send(unsigned char *ppayload, LPALL_flag flag);
	CARPCache* getcache();
	CARPmodule();
	virtual ~CARPmodule();
private:
	BOOL Comip(IP_ADDR ip1, IP_ADDR ip2);
	CARPCache m_cache;
	CBaseLayer *pethlayer;
	CBaseLayer *piplayer;
	void ResetHeader();
	ETHERNET_ADDR ethbroad;
	
	IP_ADDR ipbroad;
	IP_ADDR *ipsrc;
};

#endif 
