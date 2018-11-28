// NILayer.cpp: implementation of the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ipc.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNILayer::CNILayer(char* pName, pcap_t* pAdapterObject,
		int iNumAdapter): CBaseLayer( pName )
{
	m_pAdapterObjects = pAdapterObject;
	m_iNumAdapter = iNumAdapter;
	SetNICList();
}

CNILayer::~CNILayer()
{

}

void CNILayer::SetNICList()	// Retrieve the device list
{
	pcap_if_t *d;
	char errbuf[256];

	// pcap_findalldevs(find all of device)
	if(pcap_findalldevs(&m_allDevs, errbuf) == -1){
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	// count addapter's num
	for(d = m_allDevs; d; d=d->next)
		m_iNumAdapter++;
}

void CNILayer::setAdapterNum(int index)
{
	char errbuf[256];
	// get addapterName in m_adapterName value
	getAdapterName(index);
	
	if ((m_pAdapterObjects = pcap_open_live(m_adapterName,	// name of the device
							 65536,			// portion of the packet to capture. 
											// 65536 grants that the whole packet will be captured on all the MACs.
							 1,				// promiscuous mode (nonzero means promiscuous)
							 1000,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
    {
        fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n");
		exit(1);        // Free the device list
    }
	// save adapter's number
	m_selectedNum = index;
}

int CNILayer::getAdapterNum()//get adapter number
{
	return m_iNumAdapter;
}

char* CNILayer::getAdapterName(int index)//get adapter name
{
	pcap_if_t *d = m_allDevs;

	for(int i=0; i<index; i++)
		d = d->next;

	strcpy(m_adapterName, d->name);

	return m_adapterName;
}

void CNILayer::setMacAddress()
{
	LPADAPTER lpAdapter = 0;
	DWORD dwErrorCode;
	PPACKET_OID_DATA OidData;
	BOOL Status;

	getAdapterName(m_selectedNum);//get adapter name
	lpAdapter = PacketOpenAdapter(m_adapterName);//open adapter

	if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE)){	
		dwErrorCode=GetLastError();	// if fail to open
	}

	OidData = (PPACKET_OID_DATA) malloc(6 + sizeof(PACKET_OID_DATA));// Allocate a buffer to get the MAC adress
	if (OidData == NULL) {
		PacketCloseAdapter(lpAdapter);// fail to malloc
	}

	OidData->Oid = OID_802_3_CURRENT_ADDRESS;// Retrieve the adapter MAC querying the NIC driver
	OidData->Length = 6;
	ZeroMemory(OidData->Data, 6);
	
	Status = PacketRequest(lpAdapter, FALSE, OidData);

	if(Status){//retrun value copy OidData to m_MacAddress
		memcpy(m_MacAddress, (OidData->Data), 6);
	}
	else{
		printf("error retrieving the MAC address of the adapter!\n");
	}

	free(OidData);//free OidData
	PacketCloseAdapter(lpAdapter);//close adapter
}

unsigned char* CNILayer::getMacAddress()
{
	return m_MacAddress;//getMacAddress
}

BOOL CNILayer::Send(unsigned char* ppayload, int nlength)
{
	OID_GEN_MEDIA_CONNECT_STATUS;
	//pcap_sendpacket(send packet)
	if(pcap_sendpacket(m_pAdapterObjects, ppayload, nlength) != 0)
	{
		fprintf(stderr, "\nError sending the packet: \n", pcap_geterr(m_pAdapterObjects));
		return FALSE;
	}

	return TRUE;
}

unsigned char* CNILayer::Receive()
{
	int result;
	struct pcap_pkthdr *header;
	unsigned char data[ETHER_MAX_SIZE];
	const unsigned char* pkt_data;
	//make packet's memory
	memset(data, '\0', ETHER_MAX_SIZE);
	//pcap_next_ex(receive packet)
	while((result = pcap_next_ex( m_pAdapterObjects, &header, &pkt_data)) >= 0) {
		if(result == 0)
			continue;
		
		memcpy(data, pkt_data, ETHER_MAX_SIZE);//copy packet
		return data;//return packet
	}

	if(result = -1) {//error
		printf("Error reading the packets : %s\n", pcap_geterr(m_pAdapterObjects));
		return 0;
	}

	return 0;
}