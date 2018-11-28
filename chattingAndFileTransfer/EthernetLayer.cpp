// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "EthernetLayer.h"
#include "NILayer.h"
#include "IPCAppDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer(char* pName)
: CBaseLayer(pName)
{
	ResetHeader();
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset(m_sHeader.enet_dstaddr, 0, 6);
	memset(m_sHeader.enet_srcaddr, 0, 6);
	memset(m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	m_sHeader.enet_type = 0;
}
//return src
unsigned char* CEthernetLayer::GetSourceAddress()
{
	return m_sHeader.enet_srcaddr;
}
//return des
unsigned char* CEthernetLayer::GetDestinAddress()
{
	return m_sHeader.enet_dstaddr;
}
//src setting
void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_srcaddr, pAddress, 6);
}
//des setting
void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.enet_dstaddr, pAddress, 6);
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength)
{
	memset(m_sHeader.enet_data, 0, ETHER_MAX_DATA_SIZE);
	memcpy(m_sHeader.enet_data, ppayload, nlength);
	BOOL bSuccess = FALSE;

	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader, nlength + ETHER_HEADER_SIZE);

	return bSuccess;
}

unsigned char* CEthernetLayer::Receive()
{
	unsigned char* ppayload = mp_UnderLayer->Receive();

	unsigned char	ed[6];
	unsigned char	es[6];
	memset(ed, 0, 6);
	memset(es, 0, 6);

	if (ppayload != NULL){
		PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload;
		unsigned char F = 255;// = 0xff(breadcast)
		memcpy(ed, pFrame->enet_dstaddr, 6);
		memcpy(es, pFrame->enet_srcaddr, 6);

		int i, check = 0;
		for (i = 0; i < 6; i++)  //Comapre packet's adress and My MacAdress
		if ((ed[i] != m_sHeader.enet_srcaddr[i]) && (ed[i] != F|| (es[i] == m_sHeader.enet_srcaddr[i]))){//des and MacAdress comapre + des and F(broadcast) comapre + src and MacAdress comapre
			check = 1;
			break;
		}
		if (!check){  //is sucess to comapre and go upper layer
			return pFrame->enet_data;
		}
	}
	return 0;
}

