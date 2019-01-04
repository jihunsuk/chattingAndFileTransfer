// LayerManager.h: interface for the CLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYERMANAGER_H__D9F8CF34_8A6D_425A_BDB9_47A4874FF902__INCLUDED_)
#define AFX_LAYERMANAGER_H__D9F8CF34_8A6D_425A_BDB9_47A4874FF902__INCLUDED_

#include "BaseLayer.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLayerManager  
{
private:
	typedef struct _NODE {  // node 구조체 

		char			token[ 50 ] ;
		struct _NODE*	next ;

	} NODE, *PNODE ; // node 구조체 포인터 타입 

public:
	void			DeAllocLayer( ); // 레이어들 메모리 반환 

	void			ConnectLayers( char* pcList ); // 레이어들 사이에 연결 리스트 만들기 
	CBaseLayer*		GetLayer( char* pName ); // 이름으로 레이어 반환 
	CBaseLayer*		GetLayer( int nindex ); // 인덱스에 따라 레이어 반환
	void			AddLayer( CBaseLayer* pLayer ); // 레이어 추가 

	CLayerManager( ) ; // 생성자 
	virtual ~CLayerManager( ) ; // 소멸자 

private:
	// about stack...
	int				m_nTop; //스택의 탑 
	CBaseLayer*		mp_Stack[ MAX_LAYER_NUMBER ]; //레이어 스택 

	CBaseLayer*		Top( ); // 탑에 있는 레이어 반환 
	CBaseLayer*		Pop( ); // 제일 위에 있는 레이어를 팝 
	void			Push( CBaseLayer* pLayer ); // 현재의 탑 위에 레이어 추가 

    PNODE			mp_sListHead; // 레이어 리스트의 처음 
	PNODE			mp_sListTail; // 레이어 리스트의 끝

	// about Link Layer...
	void			LinkLayer( PNODE pNode ); // 레이어 연결 

	inline void		AddNode( PNODE pNode ); // 노드를 테일에 붙임 
	inline PNODE	AllocNode( char* pcName ); // 문자열로 노드를 구성 
	void			MakeList( char* pcList ); // 리스트 만들기 

	int				m_nLayerCount; // 레이어 수 
	CBaseLayer*		mp_aLayers[ MAX_LAYER_NUMBER ] ; // 레이어 배열 

};

#endif // !defined(AFX_LAYERMANAGER_H__D9F8CF34_8A6D_425A_BDB9_47A4874FF902__INCLUDED_)
