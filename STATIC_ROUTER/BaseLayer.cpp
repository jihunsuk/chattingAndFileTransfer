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
: m_nUpperLayerCount( 0 ), // 상위 레이어 수를 0으로 생성시 초기화 
  mp_UnderLayer( NULL ) // 하위 레이어를 NULL 로 생성시 초기화 
{
	m_pLayerName = pName ; // 받아온 이름으로 레이어 이름 지정
}

CBaseLayer::~CBaseLayer()
{

}

void CBaseLayer::SetUnderUpperLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null, // 받아온 포인터가 널이면
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderUpperLayer] The variable , 'pUULayer' is NULL" ) ;
#endif // 디버그 컴파일시 에러 메세지를 출력하고 리턴 
		return ;			
	}
}

void CBaseLayer::SetUpperUnderLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null,  // 받아온 포인터가 널이면
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperUnderLayer] The variable , 'pUULayer' is NULL" ) ;
#endif // 디버그 컴파일시 에러 메세지를 출력하고 리턴 
		return ;			
	}
	pUULayer->SetUnderLayer(this);
	//받아온 레이어의 하위 레이어에 자신 등록
	this->SetUpperLayer(pUULayer);
	//자신의 상위 레이어로 받아온 레이어 등록	
}

void CBaseLayer::SetUpperLayer(CBaseLayer *pUpperLayer )
{
	if ( !pUpperLayer ) // if the pointer is null, // 받아온 포인터 가 널이면
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperLayer] The variable , 'pUpperLayer' is NULL" ) ;
#endif // 디버그 컴파일시 에러 메세지를 출력하고 리턴 
		return ;			
	}

	// UpperLayer is added..
	this->mp_aUpperLayer[ m_nUpperLayerCount++ ] = pUpperLayer ; 
	// 상위레이어 배열에 받아온 레이어를 순서대로 지정
}

void CBaseLayer::SetUnderLayer(CBaseLayer *pUnderLayer)
{
	if ( !pUnderLayer ) // if the pointer is null, // 받아온 포인터 가 널이면
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderLayer] The variable , 'pUnderLayer' is NULL\n" ) ;
#endif // 디버그 컴파일시 에러 메세지를 출력하고 리턴 
		return ;			
	}

	// UnderLayer assignment..
	this->mp_UnderLayer = pUnderLayer ;
	// 하위레이어에 받아온 레이어를 지정
}

CBaseLayer* CBaseLayer::GetUpperLayer(int nindex)
{
	if ( nindex < 0						|| // 인덱스가 0보다 작거나 
		 nindex > m_nUpperLayerCount	|| // 인덱스가 상위 레이어 수보다 크거나 
		 m_nUpperLayerCount < 0			) // 아직 상위 레이어가 없을 경우 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUpperLayer] There is no UpperLayer in Array..\n" ) ;
#endif // 디버그 컴파일시 에러 메세지를 출력하고 널포인터 리턴 
		return NULL ;
	}

	return mp_aUpperLayer[ nindex ] ; // 인덱스에 맞는 상위 레이어 포인터 리턴
}

CBaseLayer* CBaseLayer::GetUnderLayer()
{
	if ( !mp_UnderLayer ) // 하위레이어 포인터가 널일경우 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUnderLayer] There is not a UnerLayer..\n" ) ;
#endif // 디버그 컴파일시 에러 메세지를 출력하고 널포인터 리턴 
		return NULL ;
	}

	return mp_UnderLayer ; // 하위레이어 포인터 리턴
}

char* CBaseLayer::GetLayerName()
{
	return m_pLayerName ; // 레이어 이름 리턴
}

