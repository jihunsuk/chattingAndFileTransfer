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
	typedef struct _NODE {  // node ����ü 

		char			token[ 50 ] ;
		struct _NODE*	next ;

	} NODE, *PNODE ; // node ����ü ������ Ÿ�� 

public:
	void			DeAllocLayer( ); // ���̾�� �޸� ��ȯ 

	void			ConnectLayers( char* pcList ); // ���̾�� ���̿� ���� ����Ʈ ����� 
	CBaseLayer*		GetLayer( char* pName ); // �̸����� ���̾� ��ȯ 
	CBaseLayer*		GetLayer( int nindex ); // �ε����� ���� ���̾� ��ȯ
	void			AddLayer( CBaseLayer* pLayer ); // ���̾� �߰� 

	CLayerManager( ) ; // ������ 
	virtual ~CLayerManager( ) ; // �Ҹ��� 

private:
	// about stack...
	int				m_nTop; //������ ž 
	CBaseLayer*		mp_Stack[ MAX_LAYER_NUMBER ]; //���̾� ���� 

	CBaseLayer*		Top( ); // ž�� �ִ� ���̾� ��ȯ 
	CBaseLayer*		Pop( ); // ���� ���� �ִ� ���̾ �� 
	void			Push( CBaseLayer* pLayer ); // ������ ž ���� ���̾� �߰� 

    PNODE			mp_sListHead; // ���̾� ����Ʈ�� ó�� 
	PNODE			mp_sListTail; // ���̾� ����Ʈ�� ��

	// about Link Layer...
	void			LinkLayer( PNODE pNode ); // ���̾� ���� 

	inline void		AddNode( PNODE pNode ); // ��带 ���Ͽ� ���� 
	inline PNODE	AllocNode( char* pcName ); // ���ڿ��� ��带 ���� 
	void			MakeList( char* pcList ); // ����Ʈ ����� 

	int				m_nLayerCount; // ���̾� �� 
	CBaseLayer*		mp_aLayers[ MAX_LAYER_NUMBER ] ; // ���̾� �迭 

};

#endif // !defined(AFX_LAYERMANAGER_H__D9F8CF34_8A6D_425A_BDB9_47A4874FF902__INCLUDED_)
