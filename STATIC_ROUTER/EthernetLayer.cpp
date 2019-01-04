#include "stdafx.h"
#include "EthernetLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName ) // ������ ���̽� ���̾� �����ڸ� ȣ�� 
{
	ResetHeader( ) ; // ����� ����
	memset(&broad, 0xFF, 6);
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
}

ETHERNET_ADDR CEthernetLayer::GetSourceAddress()
{
	return ethsrc; // �ڽ��� �̴��� �ּҸ� ��ȯ
}

void CEthernetLayer::SetSourceAddress(ETHERNET_ADDR pAddress)
{
	ethsrc = pAddress;
	//�ڽ��� �ּҿ� �޾ƿ°� ���� 
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, LPALL_flag flag)
{
	ppayload = (UCHAR*)(unsigned long)ppayload - (ETH_HSIZE);
	LPETHERNET	m_sHeader = (LPETHERNET)ppayload;//���
	BOOL bSuc = false;

	if(m_ARP.getcache()->getARPCache(flag->desip))
		m_sHeader->enet_desaddr = m_ARP.getcache()->getARPCache(flag->desip)->ETHaddr;
	else
		m_sHeader->enet_desaddr = broad;
	m_sHeader->enet_srcaddr = ethsrc;
	// ���� �̴��� �����͸� ���� ���̾�� �޾ƿ� ������ ����
	m_sHeader->enet_type = htons(flag->app_type); // ���� ���������� ����
	flag->app_len += ETH_HSIZE;
	// ���� ���� ���̾�� ��� + �޼��� �� ����� ����
	bSuc = mp_UnderLayer->Send(ppayload, flag); // ���� ���̾�� �޾ƿ� ��ȯ���� ����
	return bSuc;
}

BOOL CEthernetLayer::Receive( unsigned char* ppayload, LPALL_flag flag)
{
	BOOL bSuccess = false ;
	ppayload = (UCHAR*)(unsigned long)ppayload - (ETH_HSIZE);
	if(mp_UnderLayer->Receive(ppayload, flag))
	{
		LPETHERNET pFrame = (LPETHERNET)ppayload;
		flag->app_type = ntohs(pFrame->enet_type); // �÷��׿� ������ � ������������ ����
		if((addrcom(pFrame->enet_desaddr, broad) || addrcom(pFrame->enet_desaddr, ethsrc))
			&& !addrcom(pFrame->enet_srcaddr, ethsrc))
			// ��ε� ĳ��Ʈ �ų� ������ �°��� �޾� ���̳� ���� �������� �ɷ���
		{
			if(flag->app_type == ETHERTYPE_ARP)
			{
				return !m_ARP.getARPpacket(ppayload, flag);
			}
			else if(flag->app_type == ETHERTYPE_IP)
				return true;
		}
	}
	flag->app_type = 0;
	return false;
}

// �ƾ�巹�� ��
BOOL CEthernetLayer::addrcom(ETHERNET_ADDR adr1, ETHERNET_ADDR adr2) 
{
	for(int i = 0; i < 6; i++)
	{// ���� ���� �� ���� �˻� 
		if(adr1.s_ether_addr[i] != adr2.s_ether_addr[i])
			return false;
	}
	return true;
}
