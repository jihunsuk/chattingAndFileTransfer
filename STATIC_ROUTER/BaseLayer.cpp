// BaseLayer.cpp: implementation of the CBaseLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BaseLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseLayer::CBaseLayer( char* pName ) 
: m_nUpperLayerCount( 0 ), // ���� ���̾� ���� 0���� ������ �ʱ�ȭ 
  mp_UnderLayer( NULL ) // ���� ���̾ NULL �� ������ �ʱ�ȭ 
{
	m_pLayerName = pName ; // �޾ƿ� �̸����� ���̾� �̸� ����
}

CBaseLayer::~CBaseLayer()
{

}

void CBaseLayer::SetUnderUpperLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null, // �޾ƿ� �����Ͱ� ���̸�
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderUpperLayer] The variable , 'pUULayer' is NULL" ) ;
#endif // ����� �����Ͻ� ���� �޼����� ����ϰ� ���� 
		return ;			
	}
}

void CBaseLayer::SetUpperUnderLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null,  // �޾ƿ� �����Ͱ� ���̸�
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperUnderLayer] The variable , 'pUULayer' is NULL" ) ;
#endif // ����� �����Ͻ� ���� �޼����� ����ϰ� ���� 
		return ;			
	}
	pUULayer->SetUnderLayer(this);
	//�޾ƿ� ���̾��� ���� ���̾ �ڽ� ���
	this->SetUpperLayer(pUULayer);
	//�ڽ��� ���� ���̾�� �޾ƿ� ���̾� ���	
}

void CBaseLayer::SetUpperLayer(CBaseLayer *pUpperLayer )
{
	if ( !pUpperLayer ) // if the pointer is null, // �޾ƿ� ������ �� ���̸�
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperLayer] The variable , 'pUpperLayer' is NULL" ) ;
#endif // ����� �����Ͻ� ���� �޼����� ����ϰ� ���� 
		return ;			
	}

	// UpperLayer is added..
	this->mp_aUpperLayer[ m_nUpperLayerCount++ ] = pUpperLayer ; 
	// �������̾� �迭�� �޾ƿ� ���̾ ������� ����
}

void CBaseLayer::SetUnderLayer(CBaseLayer *pUnderLayer)
{
	if ( !pUnderLayer ) // if the pointer is null, // �޾ƿ� ������ �� ���̸�
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderLayer] The variable , 'pUnderLayer' is NULL\n" ) ;
#endif // ����� �����Ͻ� ���� �޼����� ����ϰ� ���� 
		return ;			
	}

	// UnderLayer assignment..
	this->mp_UnderLayer = pUnderLayer ;
	// �������̾ �޾ƿ� ���̾ ����
}

CBaseLayer* CBaseLayer::GetUpperLayer(int nindex)
{
	if ( nindex < 0						|| // �ε����� 0���� �۰ų� 
		 nindex > m_nUpperLayerCount	|| // �ε����� ���� ���̾� ������ ũ�ų� 
		 m_nUpperLayerCount < 0			) // ���� ���� ���̾ ���� ��� 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUpperLayer] There is no UpperLayer in Array..\n" ) ;
#endif // ����� �����Ͻ� ���� �޼����� ����ϰ� �������� ���� 
		return NULL ;
	}

	return mp_aUpperLayer[ nindex ] ; // �ε����� �´� ���� ���̾� ������ ����
}

CBaseLayer* CBaseLayer::GetUnderLayer()
{
	if ( !mp_UnderLayer ) // �������̾� �����Ͱ� ���ϰ�� 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUnderLayer] There is not a UnerLayer..\n" ) ;
#endif // ����� �����Ͻ� ���� �޼����� ����ϰ� �������� ���� 
		return NULL ;
	}

	return mp_UnderLayer ; // �������̾� ������ ����
}

char* CBaseLayer::GetLayerName()
{
	return m_pLayerName ; // ���̾� �̸� ����
}

