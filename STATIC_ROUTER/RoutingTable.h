#if !defined(AFX_ROUTINGTABLE_H__CAB98867_B3CB_4C73_89E1_179468087E30__INCLUDED_)
#define AFX_ROUTINGTABLE_H__CAB98867_B3CB_4C73_89E1_179468087E30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

// ��� ��ȿ����
#define UP_FLAG 0x01
// ��ι��� : �����
#define GATE_FLAG 0x02
// ��ι��� : ȣ��Ʈ
#define HOST_FLAG 0x04

// ����� ���̺�
typedef struct ROUT_TABLE_ITEM{
	// �����ּ�
	IP_ADDR des;
	// ����ũ ����
	IP_ADDR netmask;
	// ����Ʈ���� �ּ�
	IP_ADDR gateway;
	// flag : ����� ���� �� ��ġ
	char flag;
	// �̴��� ī�� ����
	int ethcard;
	// ������ġ������ Hop ����
	int metric;
}ROUT_TABLE_ITEM;

// ����� ���̺� Ŭ����
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
