// ChatAppLayer.cpp: implementation of the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "ChatAppLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE [] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CChatAppLayer::CChatAppLayer(char* pName)
: CBaseLayer(pName),
mp_Dlg(NULL)
{
	ResetHeader();
}

CChatAppLayer::~CChatAppLayer()
{

}

void CChatAppLayer::ResetHeader()
{
	m_sHeader.app_unused=0x00;
	m_sHeader.app_length = 0x0000;
	m_sHeader.app_type = 0x00;
	memset(m_sHeader.app_data, 0, APP_DATA_SIZE);

}

BOOL CChatAppLayer::Send(unsigned char *ppayload, int nlength, int port, unsigned char type)
{
	m_sHeader.app_length = (unsigned short) nlength;
	BOOL bSuccess = FALSE;

	m_sHeader.app_type = type;

	memcpy(m_sHeader.app_data, ppayload, nlength);

	bSuccess = mp_UnderLayer->Send((unsigned char*) &m_sHeader, nlength + APP_HEADER_SIZE, port);

	return bSuccess;
}

unsigned char* CChatAppLayer::Receive(int Thread_type)//Chat
{
	unsigned char* ppayload = mp_UnderLayer->Receive(Thread_type);

	if (ppayload != NULL){
		PCHAT_APP_HEADER app_hdr = (PCHAT_APP_HEADER) ppayload;
		int length = app_hdr->app_length;


		memset(chatbuf, '\0', APP_DATA_SIZE + 2);
		memcpy(chatbuf, app_hdr->app_data, length);

		if (app_hdr->app_type == CHAT_REC){//REC type
			chatbuf[APP_DATA_SIZE + 1] = 1;//masking 1
			return (unsigned char*) chatbuf;
		}
		else if (app_hdr->app_type == CHAT_NOTREC){//NOTREC type
			chatbuf[APP_DATA_SIZE + 1] = 0;//masking 0
			return (unsigned char*) chatbuf;
		}
	}
	return 0;

}


