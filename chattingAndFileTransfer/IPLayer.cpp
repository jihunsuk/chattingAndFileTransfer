// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "IPLayer.h"
#include "NILayer.h"
#include "IPCAppDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPLayer::CIPLayer(char* pName)
: CBaseLayer(pName)
{
	ResetHeader();
}

CIPLayer::~CIPLayer()
{
}

void CIPLayer::ResetHeader()
{
	memset(m_sHeader.ip_dstaddr, 0, 4);
	memset(m_sHeader.ip_srcaddr, 0, 4);
	memset(m_sHeader.ip_data, 0, IP_DATA_SIZE);
	m_sHeader.ip_type = 0;
}
//return src
unsigned char* CIPLayer::GetSourceAddress()
{
	return m_sHeader.ip_srcaddr;
}
//return des 
unsigned char* CIPLayer::GetDestinAddress()
{
	return m_sHeader.ip_dstaddr;
}
//set src
void CIPLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.ip_srcaddr, pAddress, 4);
}
//set des
void CIPLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.ip_dstaddr, pAddress, 4);
}

BOOL CIPLayer::Send(unsigned char *ppayload, int nlength)
{
	memset(m_sHeader.ip_data, 0, IP_DATA_SIZE);
	memcpy(m_sHeader.ip_data, ppayload, nlength);
	BOOL bSuccess = FALSE;

	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader, nlength + IP_HEADER_SIZE);

	return bSuccess;
}

unsigned char* CIPLayer::Receive()
{
	unsigned char* ppayload = mp_UnderLayer->Receive();

	if (ppayload != NULL){
		PIP_HEADER pFrame = (PIP_HEADER) ppayload;

		return pFrame->ip_data;
	}
	return 0;
}