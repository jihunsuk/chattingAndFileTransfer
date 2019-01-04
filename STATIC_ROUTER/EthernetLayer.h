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
: public CBaseLayer  // ���̽� ���̾� ���
{
private:
	inline void		ResetHeader( ); //��� ����

public:
	BOOL			Receive( unsigned char* ppayload, LPALL_flag flag) ; // ���� ���̾ �޼��� ���� 
	BOOL			Send( unsigned char* ppayload, LPALL_flag flag); // ���� ���̾�� �޼��� ���� 
	void			SetSourceAddress( ETHERNET_ADDR pAddress ); // �۽� �ּ� ����
	ETHERNET_ADDR	GetSourceAddress( ) ; // �۽� �ּ� ��� 
	CARPmodule m_ARP;
	CEthernetLayer( char* pName ); // ������ 
	virtual ~CEthernetLayer(); // �Ҹ��� 
	ETHERNET_ADDR broad;	// ��ε� ĳ��Ʈ ���� 	
	ETHERNET_ADDR ethsrc;	// �� mac �ּ�
protected:
	
private:
	BOOL addrcom(ETHERNET_ADDR adr1, ETHERNET_ADDR adr2); // �ƾ�巹���� �� �ϱ� ���� �Լ� 

};

#endif // !defined(AFX_ETHERNETLAYER_H__7857C9C2_B459_4DC8_B9B3_4E6C8B587B29__INCLUDED_)
