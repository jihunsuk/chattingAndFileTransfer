#include "stdafx.h"
#include "NILayer.h"
#include "EthernetLayer.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define OID_802_3_CURRENT_ADDRESS 0x01010102	// �ƾ�巹�� �޴µ� �ʿ��� 
#define PCAP_OPENFLAG_PROMISCUOUS 1

CNILayer::CNILayer( char* pName ) 
: CBaseLayer( pName )// ������ ��ӹ��� ���̽� ���̾ �Բ� ����
{
	eth = NULL;
	if(pcap_findalldevs(&alldevs, errbuf) < 0)	// �̴��� ī�� �޾ƿ���
	{
		AfxMessageBox("��Ʈ�� ��� ã�� �� �����ϴ�.");
		exit(-1);
	}	
}

CNILayer::~CNILayer()
{
	closeETH(); // ������ pcap ��ũ���� ���� 
	pcap_freealldevs(alldevs);	// ���� �̴��� ���� �޸� ���� 
}

BOOL CNILayer::Send(unsigned char *ppayload, LPALL_flag flag)
{	// pcap_sendpacket�� �̿��ؼ� �̴��� ī�忡 ��Ŷ�� ����
	return !pcap_sendpacket(eth, ppayload, flag->app_len);
}

// pcap_next_ex�� �̿��� �̴��� ī�忡�� �� ��Ŷ�� ���� 
BOOL CNILayer::Receive(unsigned char *ppayload, LPALL_flag flag)
{	
	const UCHAR *buf;	// ���� ��Ŷ�� �޾ƿ� ���� next_ex �� �޸� �ڵ� �Ҵ�
	struct pcap_pkthdr *header;	//��Ŷ ������ ���� ��� 
	
	if((pcap_next_ex(eth, &header, (const UCHAR **)&buf)) > 0)
	{
		flag->ethsel = ethsel;
		memcpy(ppayload, buf, 1514);
		return true;// �̴��� �ִ� ũ�� ���� 
	}
	return false;	// ��Ŷ �޾� ���µ� ���� 
}

// �־��� �̴��� ī�带 ���� 
void CNILayer::setETH(int sel)
{
	ethsel = sel;	
	pcap_if_t *trail;	// alldevs �� ��ȸ
	trail = alldevs;	// alldevs ���� ����
	int i = 0;			// sel �� ������ ���ϱ� ���Ͽ� 
	while(trail)
	{		
		if((trail->flags != PCAP_IF_LOOPBACK) && (trail->addresses != NULL))
        {// ���� ���� �ƴϰ� �ּҰ� ���� �ƴϸ� (�����ڿ��� �޾ƿ��� ���ǰ� ����)
			if(i == sel)	// ������ ���� ������
				break;		// ���� Ż��
			i++;	// ���� �ڸ��� �̵�
        }
		trail = trail->next;	// alldevs �� �����ڸ��� �̵�
	}
	eth = pcap_open_live(trail->name, 1514, PCAP_OPENFLAG_PROMISCUOUS, 1, NULL);
	// ������ ũ�⸦ 1514, ���ι̽� Ŀ�� ��� Ÿ�Ӿƿ��� 1ms���� ������ ��͸���
	
	PACKET_OID_DATA* mac;	// ���ۿ� �Լ��� �ѱ� �������� ������
	UCHAR buf[512];	// ���� mac ��巹���� ����� ���� 
	mac = (PACKET_OID_DATA*)buf;
	mac->Oid = OID_802_3_CURRENT_ADDRESS;
	mac->Length = 6;
	struct sockaddr_in *si;	// ip�� ��� ���� ���� �ʿ��� ���� ���� ����ü 
	si = (struct sockaddr_in *)trail->addresses->addr;	// �ּ������� Ÿ�� ĳ������ 
	LPADAPTER lpAdapter;	// mac ��� ���µ� �ʿ��� PacketRequest �Լ� ��
	lpAdapter = PacketOpenAdapter(trail->name);
	PacketRequest(lpAdapter, FALSE, mac);	// mac ��巹�� ������
	memcpy(macad.s_ether_addr, mac->Data, mac->Length);
	memcpy(&ipad, &si->sin_addr.S_un.S_addr, 4);
	PacketCloseAdapter(lpAdapter); // ����� ��� ���� 

	((CEthernetLayer *)mp_aUpperLayer[0])->SetSourceAddress(macad);
	((CIPLayer *)mp_aUpperLayer[0]->GetUpperLayer(0))->SetSourceAddress(ipad);
}

ETHERNET_ADDR CNILayer::getMacAddress()
{
	return macad;	// �迭�� �� ��巹�� ��ȯ
}

IP_ADDR CNILayer::getIPAddress()
{
	return ipad;	// ip ��巹�� ��ȯ
}

void CNILayer::closeETH()
{
	if(eth)	// NULL �� �ƴϸ� (�̹� ���� ������)
	{
		pcap_close(eth);	// ��� ���� 
		eth = NULL;			// ����� Ȯ���� ���� NULL �� ����
	}
}