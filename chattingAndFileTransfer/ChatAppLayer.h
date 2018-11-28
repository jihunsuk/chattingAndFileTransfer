// ChatAppLayer.h: interface for the CChatAppLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)
#define AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CChatAppLayer 
: public CBaseLayer  
{
private:
	inline void		ResetHeader( ) ;
	CObject* mp_Dlg ;

public:
	unsigned char*	Receive(int Thread_type);
	BOOL			Send(unsigned char* ppayload, int nlength, int port, unsigned char type);

	CChatAppLayer( char* pName );
	virtual ~CChatAppLayer();

	typedef struct _CHAT_APP_HEADER {

		unsigned short	app_length	; // total length of the data
		unsigned char	app_unused;	//unused
		unsigned char	app_type; // type of application data (2 is notREC , 1 is REC)
		unsigned char	app_data[ APP_DATA_SIZE ] ; // application data

	} CHAT_APP_HEADER, *PCHAT_APP_HEADER ;

	unsigned char chatbuf[APP_DATA_SIZE+2];//chat buff()

protected:
	CHAT_APP_HEADER		m_sHeader ;

	enum {			DATA_TYPE_CONT = 0x01, //not used.. we make another define in stdafx.h
					DATA_TYPE_END =  0x02 } ;
};

#endif // !defined(AFX_CHATAPPLAYER_H__E78615DE_0F23_41A9_B814_34E2B3697EF2__INCLUDED_)










