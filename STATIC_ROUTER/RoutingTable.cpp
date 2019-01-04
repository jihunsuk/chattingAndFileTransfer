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

// 생성자
CRoutingTable::CRoutingTable(){
	entrycnt = 0; // 테이블내 데이터 갯수 초기화
}

// 소멸자
CRoutingTable::~CRoutingTable(){}

// 라우터 테이블 아이템 번호 반환
int CRoutingTable::getEntryCnt(){
	return entrycnt;
}

// 라우터 테이블 아이템 추가
void CRoutingTable::addEntry(ROUT_TABLE_ITEM &item){
	entry[entrycnt++] = item;
	sort();
}

// 아이템 삭제
void CRoutingTable::delEntry(int &index){
	int i;
	for(i = index; i < entrycnt; i++){
		entry[i] = entry[i + 1];
	}
	entrycnt--;
}

// 라우터 테이블의 아이템 정렬(=내림차순)
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
			// 내림차순 정렬
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

// 아이템 주소값 반환
ROUT_TABLE_ITEM* CRoutingTable::getEntry(int &index){
	return &entry[index];
}
