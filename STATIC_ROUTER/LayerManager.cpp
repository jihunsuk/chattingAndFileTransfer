#include "stdafx.h"
#include "LayerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CLayerManager::CLayerManager()
: m_nLayerCount( 0 ), // ������ ���� �ʱ�ȭ 
  mp_sListHead( NULL ),
  mp_sListTail( NULL ),
  m_nTop( -1 ) // ������ ž -1 �� ����
{

}

CLayerManager::~CLayerManager()
{
	PNODE temp;		// �޸� ���� Ȯ���ؼ� �߰� ��
	while(mp_sListHead)
	{
		temp = mp_sListHead;
		mp_sListHead = mp_sListHead->next;
		delete temp;
	}
}

void CLayerManager::AddLayer(CBaseLayer *pLayer)
{
	mp_aLayers[ m_nLayerCount++ ] = pLayer ; // ���̾� �� �޾ƿ� ���̾� �߰�
}

CBaseLayer* CLayerManager::GetLayer(int nindex)
{
	return mp_aLayers[ nindex ] ; // ���̾� ��ȯ
}

CBaseLayer* CLayerManager::GetLayer(char *pName)
{
	for ( int i = 0 ; i < m_nLayerCount ; i++ ) //���̾� �� ���� �ݺ�
	{
		CString stName( pName ) ;
		if ( !stName.Compare( mp_aLayers[i]->GetLayerName( ) ) ) // �̸��� ������ 
			return mp_aLayers[i] ; // ���� ���̾� ��ȯ
	}

	return NULL ; // ���� ��� �� ������ ��ȯ
}

void CLayerManager::ConnectLayers(char *pcList)
{
	MakeList( pcList ) ; // ����Ʈ ����� 
	LinkLayer( mp_sListHead ) ; // ��ũ ����
}

void CLayerManager::MakeList(char *pcList)
{
	while( pcList != (char*) 0x01 ) // �޾ƿ� ���� 1�� �ƴ� ���� 
	{
		char sBuff[ 100 ] ; // ���� ����
		sscanf( pcList, "%s", sBuff ) ; // �޾ƿ� ���ڿ��� sBuff ���� ����
		pcList = strchr( pcList, ' ' ) + 1 ; // pcList ���� ��ĭ�� ã�� �̵�

		PNODE pNode = AllocNode( sBuff ) ; // ��带 ���� ��
		AddNode( pNode ) ;// ���� ��带 ������ ����Ʈ�� ����
	}
}

CLayerManager::PNODE CLayerManager::AllocNode(char *pcName)
{
	PNODE node = new NODE ; // �� ��� ����
	ASSERT( node ) ; // ��尡 �ùٸ��� �˻� 

	strcpy( node->token, pcName ) ; // ����� ��ū�� �޾ƿ� ���ڿ� ����
	node->next = NULL ; // �ؽ�Ʈ�� �η� ����

	return node ; // ���� ��带 ����
}

void CLayerManager::AddNode(PNODE pNode)
{
	if ( !mp_sListHead ) // ����Ʈ�� ���� �ƴϸ�
	{
		mp_sListHead = mp_sListTail = pNode ; // ���� ���Ͽ� ���� ��� ����
	}
	else
	{ // �̹� ����Ʈ�� ���� ��� 
		mp_sListTail->next = pNode ; // ������ �ڿ� ����
		mp_sListTail = pNode ; // ������ �޾ƿ� ���� ����
	}
}


void CLayerManager::Push(CBaseLayer *pLayer)
{
	if ( m_nTop >= MAX_LAYER_NUMBER ) // ������ ���� ���� �ö��� ��� 
	{
#ifdef _DEBUG
		TRACE( "The Stack is full.. so cannot run the push operation.. \n" ) ;
#endif// ����� �����Ͻ� �޼��� ����� ����
		return ;
	}

	mp_Stack[ ++m_nTop ] = pLayer ; // ������ ���� ���� ����
}

CBaseLayer* CLayerManager::Pop()
{
	if ( m_nTop < 0 ) // ������ ����� ��� 
	{
#ifdef _DEBUG
		TRACE( "The Stack is empty.. so cannot run the pop operation.. \n" ) ;
#endif// ����� �����Ͻ� �޼��� ����� ����
		return NULL ;
	}

	CBaseLayer* pLayer = mp_Stack[ m_nTop ] ; // ������ ž�� ����
	mp_Stack[ m_nTop ] = NULL ; // ������ ž�� �� ����
	m_nTop-- ; // ž�� 1 ����

	return pLayer ; // ž�� �ִ� �����͸� ��ȯ
}

CBaseLayer* CLayerManager::Top()
{
	if ( m_nTop < 0 ) // ������ ���������
	{
#ifdef _DEBUG
		TRACE( "The Stack is empty.. so cannot run the top operation.. \n" ) ;
#endif
		return NULL ;
	}

	return mp_Stack[ m_nTop ] ; // ž ����
}

void CLayerManager::LinkLayer(PNODE pNode)
{
	CBaseLayer* pLayer = NULL ;

	while ( pNode ) // �޾ƿ� ���� ���� �ƴϸ� 
	{
		if ( !pLayer ) // ������ ���̸� 
			pLayer = GetLayer( pNode->token ) ;//�޾ƿ� ����� ��ū�� �´� ���̾� �޾ƿ�
		else
		{// ���� �ƴѰ��
			if ( *pNode->token == '(' ) //��ū�� (�� 
				Push( pLayer ) ; // ���ÿ� Ǫ�� 
			else if ( *pNode->token == ')' ) // ��ū�� ) ��
				Pop( ) ;//��
			else
			{
				char cMode = *pNode->token ; // ��ū�� ������ ����
				char* pcName = pNode->token + 1 ; // ��ū�� ���� ���� ���� pcName �� ����
				
				pLayer = GetLayer( pcName ) ; // �־��� ������ ���̾� ����

				switch( cMode ) // ��忡 ���� 
				{
				case '*' : Top( )->SetUpperUnderLayer( pLayer ) ; break ; // ������ ���̾� ����
				case '+' : Top( )->SetUpperLayer( pLayer ) ; break ; // ���� ���̾� ����
				case '-' : Top( )->SetUnderLayer( pLayer ) ; break ; // ���� ���̾� ����
				}
			}
		}

		pNode = pNode->next ; // ���� ����Ʈ�� �̵�
	}
}

void CLayerManager::DeAllocLayer()
{
	for ( int i = 0 ; i < this->m_nLayerCount ; i ++ ) // ���̾� �� ��ŭ 
		delete this->mp_aLayers[ i ] ; // ���̾� �޸𸮸� ��ȯ �Ѵ� 
}
