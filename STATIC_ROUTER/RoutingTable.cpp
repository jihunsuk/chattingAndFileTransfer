// RoutingTable.cpp: implementation of the CRoutingTable class.

#include "stdafx.h"
#include "StaticRouter.h"
#include "RoutingTable.h"
#include <WinSock2.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ������
CRoutingTable::CRoutingTable(){
	entrycnt = 0; // ���̺� ������ ���� �ʱ�ȭ
}

// �Ҹ���
CRoutingTable::~CRoutingTable(){}

// ����� ���̺� ������ ��ȣ ��ȯ
int CRoutingTable::getEntryCnt(){
	return entrycnt;
}

// ����� ���̺� ������ �߰�
void CRoutingTable::addEntry(ROUT_TABLE_ITEM &item){
	entry[entrycnt++] = item;
	sort();
}

// ������ ����
void CRoutingTable::delEntry(int &index){
	int i;
	for(i = index; i < entrycnt; i++){
		entry[i] = entry[i + 1];
	}
	entrycnt--;
}

// ����� ���̺��� ������ ����(=��������)
void CRoutingTable::sort(){
	int i;
	bool flag = true;
	ROUT_TABLE_ITEM temp;
	
	while(flag){
		flag = false;
		for(i = 0; i < entrycnt -1; i++){
			unsigned long net1, net2;
			memcpy(&net1, entry[i].netmask.s_ip_addr, 4);
			memcpy(&net2, entry[i+1].netmask.s_ip_addr, 4);
			// �������� ����
			if(net1 < net2)
			{
				temp = entry[i];
				entry[i] = entry[i + 1];
				entry[i + 1] = temp;
				flag = true;		
			}
		}
	}
}

// ������ �ּҰ� ��ȯ
ROUT_TABLE_ITEM* CRoutingTable::getEntry(int &index){
	return &entry[index];
}
