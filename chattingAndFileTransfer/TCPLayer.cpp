// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "TCPLayer.h"
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

CTCPLayer::CTCPLayer(char* pName)
: CBaseLayer(pName)
{
	ResetHeader();
	_Rock = FALSE;
}

CTCPLayer::~CTCPLayer()
{
}

void CTCPLayer::ResetHeader()
{
	memset(chat_Packet, 0, IP_DATA_SIZE);//C
	memset(file_Packet, 0, IP_DATA_SIZE);//C

	memset(m_sHeader.tcp_dstaddr, 0, 2);
	memset(m_sHeader.tcp_srcaddr, 0, 2);
	memset(m_sHeader.tcp_data, 0, TCP_DATA_SIZE);
	m_sHeader.tcp_type = 0;
}
//retrun src 
unsigned char* CTCPLayer::GetSourceAddress()
{
	return m_sHeader.tcp_srcaddr;
}
//return des
unsigned char* CTCPLayer::GetDestinAddress()
{
	return m_sHeader.tcp_dstaddr;
}
//set src 
void CTCPLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.tcp_srcaddr, pAddress, 2);
}
//set des
void CTCPLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy(m_sHeader.tcp_dstaddr, pAddress, 2);
}

BOOL CTCPLayer::Send(unsigned char *ppayload, int nlength, int port)
{
	memset(m_sHeader.tcp_data, 0, TCP_DATA_SIZE);
	memcpy(m_sHeader.tcp_data, ppayload, nlength);
	BOOL bSuccess = FALSE;

	m_sHeader.tcp_type = port;

	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader, nlength + TCP_HEADER_SIZE);

	return bSuccess;
}

unsigned char* CTCPLayer::Receive(int Thread_type)
{
	static PTCP_HEADER pFrame;
	if (Thread_type == 2){// type 2 
		unsigned char* ppayload = mp_UnderLayer->Receive();
		if (ppayload != NULL){
			pFrame = (PTCP_HEADER) ppayload;
			if ((pFrame->tcp_type == CHAT_TYPE || pFrame->tcp_type == FILE_TYPE)){
				_Rock = TRUE;
				while (_Rock);
			}
		}
	}
	else if (_Rock ){//type 0 , 1 
		if (pFrame->tcp_type == CHAT_TYPE && !Thread_type){//return to ChatAppLayer
			memcpy(chat_Packet, (unsigned char*) pFrame, IP_DATA_SIZE);//C
			PTCP_HEADER C_pFrame = (PTCP_HEADER) chat_Packet;
			_Rock = FALSE;
			return C_pFrame->tcp_data;//bSuccess = mp_aUpperLayer[0]->Receive();
		}
		else if (pFrame->tcp_type == FILE_TYPE && Thread_type){//return to FileLayer
			memcpy(file_Packet, (unsigned char*) pFrame, IP_DATA_SIZE);//C
			PTCP_HEADER F_pFrame = (PTCP_HEADER) file_Packet;
			_Rock = FALSE;
			return F_pFrame->tcp_data;
		}
	}
	return 0;
}