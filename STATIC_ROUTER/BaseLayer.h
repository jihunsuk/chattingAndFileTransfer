// BaseLayer.h: interface for the CBaseLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASELAYER_H__041C5A07_23A9_4CBC_970B_8743460A7DA9__INCLUDED_)
#define AFX_BASELAYER_H__041C5A07_23A9_4CBC_970B_8743460A7DA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include <winsock2.h>

class CBaseLayer
{
public:
	char*			GetLayerName( ); // 레이어 이름 반환
	
	CBaseLayer*		GetUnderLayer( ); // 하위 레이어 반환 
	CBaseLayer*		GetUpperLayer( int nindex ); // 인덱스 에 맞는 상위 레이어 반환
	void			SetUnderUpperLayer( CBaseLayer* pUULayer = NULL ); // 하위 상위 레이어 세팅
	void			SetUpperUnderLayer( CBaseLayer* pUULayer = NULL ); // 상위 하위 레이어 세팅 
	void			SetUnderLayer( CBaseLayer* pUnderLayer = NULL ) ; // 하위 레이어 세팅 
	void			SetUpperLayer( CBaseLayer* pUpperLayer = NULL ) ; // 상위 레이어 세팅 
	
	CBaseLayer( char* pName = NULL ); // 기본 생성자 이름이 없을 경우 NULL 로 세팅
	virtual ~CBaseLayer(); // 소멸자 
	
	// param : unsigned char*	- the data of the upperlayer
	//         int				- the length of data
	virtual	BOOL	Send(unsigned char* ppayload, LPALL_flag flag) 	{ return FALSE ; } // 하위 레이어로 메세지 보내기 
	// param : unsigned char*	- the data of the underlayer
	virtual	BOOL	Receive( unsigned char* ppayload, LPALL_flag flag) 	{ return FALSE ; }  // 상위 레이어로 메세지 보내기 
	//virtual	BOOL	Receive( ) 	{ return FALSE ; } // 상위 레이어로 메세지 보내기 
	
protected:
	char*			m_pLayerName; // 레이어 이름 
	CBaseLayer*		mp_UnderLayer;	// 하위 레이어 	                    		// UnderLayer pointer
	CBaseLayer*		mp_aUpperLayer[ MAX_LAYER_NUMBER ]; // 상위 레이어 배열		// UpperLayer pointer
	int				m_nUpperLayerCount;	// 레이어 숫자 	                    	// UpperLayer Count
};

#endif // !defined(AFX_BASELAYER_H__041C5A07_23A9_4CBC_970B_8743460A7DA9__INCLUDED_)
