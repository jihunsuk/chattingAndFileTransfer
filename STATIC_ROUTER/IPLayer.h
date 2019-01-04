#if !defined(AFX_IPLAYER_H__E0AED5E6_347A_469F_8298_5FEDF93AB448__INCLUDED_)
#define AFX_IPLAYER_H__E0AED5E6_347A_469F_8298_5FEDF93AB448__INCLUDED_

#include "RoutingTable.h"
#include "ARPmodule.h"

#if _MSC_VER > 1000
#pragma once
#endif

#include "BaseLayer.h"
#include "StdAfx.h"

class CIPLayer : public CBaseLayer  {
public:
	IP_ADDR Andip(IP_ADDR &ip1, IP_ADDR &ip2);
	void setOthIPlayer(CIPLayer *iplayer);
	void setRoutingTable(CRoutingTable *rtable);
	BOOL	Receive( unsigned char* ppayload , LPALL_flag flag);
	BOOL	Send( unsigned char* ppayload, LPALL_flag flag);
	void SetSourceAddress(IP_ADDR pAddress);
	IP_ADDR GetSourceAddress();
	void setARP(CARPmodule *parp);
	CARPmodule *mp_ARP;
	CIPLayer( char* pName );
	CIPLayer();
	virtual ~CIPLayer();
private:
	CIPLayer *mp_otherip[MAX_ETHERNET_CARD];
	int ethcnt;
	CRoutingTable *mp_rtable;	
	IP_ADDR ipsrc;
	IP_ADDR ipbroad;
	BOOL Comip(IP_ADDR &ip1, IP_ADDR &ip2);
};

typedef struct SendParam
{
	CIPLayer *iplayer;
	unsigned char *ppayload;
	LPALL_flag flag;
	bool *thrflag;
}SendParam;

#endif 
