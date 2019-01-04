#if !defined(AFX_ROUTINGTABLE_H__CAB98867_B3CB_4C73_89E1_179468087E30__INCLUDED_)
#define AFX_ROUTINGTABLE_H__CAB98867_B3CB_4C73_89E1_179468087E30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

// 경로 유효상태
#define UP_FLAG 0x01
// 경로방향 : 라우터
#define GATE_FLAG 0x02
// 경로방향 : 호스트
#define HOST_FLAG 0x04

// 라우터 테이블
typedef struct ROUT_TABLE_ITEM{
	// 목적주소
	IP_ADDR des;
	// 마스크 범위
	IP_ADDR netmask;
	// 게이트웨이 주소
	IP_ADDR gateway;
	// flag : 경로의 상태 및 위치
	char flag;
	// 이더넷 카드 정보
	int ethcard;
	// 목적위치까지의 Hop 갯수
	int metric;
}ROUT_TABLE_ITEM;

// 라우터 테이블 클레스
class CRoutingTable  
{
public:
	ROUT_TABLE_ITEM* getEntry(int &index); 
	void delEntry(int &index);
	void addEntry(ROUT_TABLE_ITEM &item);
	int getEntryCnt();
	CRoutingTable();
	virtual ~CRoutingTable();

private:
	void sort();
	ROUT_TABLE_ITEM entry[MAX_ENTRY];
	int entrycnt;
};

#endif 
