// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
#define AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "ARPmodule.h"

class CEthernetLayer 
: public CBaseLayer  // 베이스 레이어 상속
{
private:
	inline void		ResetHeader( ); //헤더 리셋

public:
	BOOL			Receive( unsigned char* ppayload, LPALL_flag flag) ; // 상위 레이어에 메세지 보냄 
	BOOL			Send( unsigned char* ppayload, LPALL_flag flag); // 하위 레이어로 메세지 보냄 
	void			SetSourceAddress( ETHERNET_ADDR pAddress ); // 송신 주소 설정
	ETHERNET_ADDR	GetSourceAddress( ) ; // 송신 주소 얻기 
	CARPmodule m_ARP;
	CEthernetLayer( char* pName ); // 생성자 
	virtual ~CEthernetLayer(); // 소멸자 
	ETHERNET_ADDR broad;	// 브로드 캐스트 모드용 	
	ETHERNET_ADDR ethsrc;	// 내 mac 주소
protected:
	
private:
	BOOL addrcom(ETHERNET_ADDR adr1, ETHERNET_ADDR adr2); // 맥어드레스를 비교 하기 위한 함수 

};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
