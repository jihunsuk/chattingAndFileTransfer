#include "stdafx.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void SendThr(SendParam *param);

CIPLayer::CIPLayer(){
	memset(&ipbroad, 0xFF, 4);
	ethcnt = 0;
}

CIPLayer::~CIPLayer(){}

CIPLayer::CIPLayer(char *pName)
: CBaseLayer( pName ){
	memset(&ipbroad, 0xFF, 4);
	ethcnt = 0;
}

void CIPLayer::setARP(CARPmodule *parp){
	mp_ARP = parp;
	mp_ARP->setData(&ipsrc);
}

// ������ ����
BOOL CIPLayer::Send(unsigned char* ppayload, LPALL_flag flag){
	// ARP�� send�� ������ ����
	return mp_ARP->send(ppayload, flag);
}

BOOL CIPLayer::Receive(unsigned char* ppayload , LPALL_flag flag){
	ppayload = (UCHAR*)(unsigned long)ppayload - IP_HSIZE;
	// �����͸� ������ ���
	if(mp_UnderLayer->Receive(ppayload, flag)){
		int i;
		ROUT_TABLE_ITEM ritem;
		// ARP�� ������ ���
		if(flag->app_type == ETHERTYPE_ARP){
			LPARP pFramearp = (LPARP)ppayload;
			// ����� ���̺��� �˻�
			for(i = 0; i < mp_rtable->getEntryCnt(); i++){
				ritem = *mp_rtable->getEntry(i);
				flag->desip = pFramearp->ip_srcaddr;
				// �������� �� ����Ʈ�� ��
				if(Comip(ritem.des, Andip(ritem.netmask, pFramearp->ip_desaddr))){	
					return !mp_ARP->sendARP(flag, ARP_OP_REPLY, &pFramearp->ip_desaddr, &pFramearp->enet_srcaddr);
				}
			}
		}
		// ARP�� �ƴ� ���
		LPIP pFrame = (LPIP)ppayload;
		flag->app_len = ntohs(pFrame->totlen);
		// �ڽ��� ���� ��Ŷ�� ���� return
		if(Comip(pFrame->ip_desaddr, ipsrc)){ return false; }
		// ����� ���̺� �˻�
		for(i = 0; i < mp_rtable->getEntryCnt(); i++){
			ritem = *mp_rtable->getEntry(i);
			// ����Ʈ������ ���
			if(ritem.flag & GATE_FLAG){
				// AND ������ ���� �ּ�Ȯ��
				if(Comip(ritem.des, Andip(ritem.netmask, pFrame->ip_desaddr))){
					SendParam param;
					ALL_flag sendflag = *flag;
					bool thrflag = true;
					// ���� IP�� ����Ʈ���̷� ����, x.x.x.0
					sendflag.desip = ritem.gateway;
					sendflag.ethsel = ritem.ethcard;
					param.flag = &sendflag;
					param.iplayer = mp_otherip[ritem.ethcard];
					param.ppayload = ppayload;
					param.thrflag = &thrflag;
					// send�� �����带 �����Ͽ� ����
					AfxBeginThread((AFX_THREADPROC)SendThr, &param);
					while(thrflag){
						Sleep(10);
					}
					return true;
				}
			}
			// ȣ��Ʈ�� ���
			else if(ritem.flag & HOST_FLAG){
				if(Comip(ritem.des, Andip(ritem.netmask, pFrame->ip_desaddr)))
				{
					SendParam param;
					ALL_flag sendflag = *flag;
					bool thrflag = true;
					sendflag.desip = pFrame->ip_desaddr;
					sendflag.ethsel = ritem.ethcard;
					param.flag = &sendflag;
					param.iplayer = mp_otherip[ritem.ethcard];
					param.ppayload = ppayload;
					param.thrflag = &thrflag;
					AfxBeginThread((AFX_THREADPROC)SendThr, &param);
					while(thrflag){
						Sleep(10);
					}
					return true;
				}
			}
		}
	}
	return false;
}

void CIPLayer::SetSourceAddress(IP_ADDR pAddress){
	ipsrc = pAddress;
}

IP_ADDR CIPLayer::GetSourceAddress(){
	return ipsrc;
}

// �ּ� ��
BOOL CIPLayer::Comip(IP_ADDR &ip1, IP_ADDR &ip2)
{
	int i;
	for(i = 0; i < 4; i++){
		if(ip1.s_ip_addr[i] != ip2.s_ip_addr[i])
			return false;
	}
	return true;
}

void CIPLayer::setRoutingTable(CRoutingTable *rtable){
	mp_rtable = rtable;
}

void CIPLayer::setOthIPlayer(CIPLayer *iplayer){
	mp_otherip[ethcnt++] = iplayer;
}

IP_ADDR CIPLayer::Andip(IP_ADDR &ip1, IP_ADDR &ip2){
	IP_ADDR m_andip;
	int i;
	for(i = 0; i < 4; i++){
		m_andip.s_ip_addr[i] = ip1.s_ip_addr[i] & ip2.s_ip_addr[i];
	}
	return m_andip;
}


void SendThr(SendParam *param){
	SendParam sparam = *param;
	ALL_flag flag = *param->flag;
	unsigned char buf[1514];
	unsigned char *ppayload = (UCHAR*)(long)buf + ETH_HSIZE;
	// ���� ��Ŷ�� ����
	memcpy(ppayload, sparam.ppayload, flag.app_len);
	*(sparam.thrflag) = false;
	// ������ ����
	param->iplayer->Send(ppayload, &flag);
}