#include "stdafx.h"
#include "LayerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CLayerManager::CLayerManager()
: m_nLayerCount( 0 ), // 생성시 변수 초기화 
  mp_sListHead( NULL ),
  mp_sListTail( NULL ),
  m_nTop( -1 ) // 스택의 탑 -1 로 설정
{

}

CLayerManager::~CLayerManager()
{
	PNODE temp;		// 메모리 누수 확인해서 추가 함
	while(mp_sListHead)
	{
		temp = mp_sListHead;
		mp_sListHead = mp_sListHead->next;
		delete temp;
	}
}

void CLayerManager::AddLayer(CBaseLayer *pLayer)
{
	mp_aLayers[ m_nLayerCount++ ] = pLayer ; // 레이어 에 받아온 레이어 추가
}

CBaseLayer* CLayerManager::GetLayer(int nindex)
{
	return mp_aLayers[ nindex ] ; // 레이어 반환
}

CBaseLayer* CLayerManager::GetLayer(char *pName)
{
	for ( int i = 0 ; i < m_nLayerCount ; i++ ) //레이어 수 까지 반복
	{
		CString stName( pName ) ;
		if ( !stName.Compare( mp_aLayers[i]->GetLayerName( ) ) ) // 이름이 같으면 
			return mp_aLayers[i] ; // 현재 레이어 반환
	}

	return NULL ; // 없을 경우 널 포인터 반환
}

void CLayerManager::ConnectLayers(char *pcList)
{
	MakeList( pcList ) ; // 리스트 만들기 
	LinkLayer( mp_sListHead ) ; // 링크 연결
}

void CLayerManager::MakeList(char *pcList)
{
	while( pcList != (char*) 0x01 ) // 받아온 값이 1이 아닌 동안 
	{
		char sBuff[ 100 ] ; // 버퍼 생성
		sscanf( pcList, "%s", sBuff ) ; // 받아온 문자열에 sBuff 내용 넣음
		pcList = strchr( pcList, ' ' ) + 1 ; // pcList 에서 빈칸을 찾아 이동

		PNODE pNode = AllocNode( sBuff ) ; // 노드를 생성 함
		AddNode( pNode ) ;// 만든 노드를 기존의 리스트에 연결
	}
}

CLayerManager::PNODE CLayerManager::AllocNode(char *pcName)
{
	PNODE node = new NODE ; // 새 노드 생성
	ASSERT( node ) ; // 노드가 올바른지 검사 

	strcpy( node->token, pcName ) ; // 노드의 토큰에 받아온 문자열 복사
	node->next = NULL ; // 넥스트를 널로 지정

	return node ; // 만든 노드를 리턴
}

void CLayerManager::AddNode(PNODE pNode)
{
	if ( !mp_sListHead ) // 리스트가 널이 아니면
	{
		mp_sListHead = mp_sListTail = pNode ; // 헤드와 테일에 현재 노드 지정
	}
	else
	{ // 이미 리스트가 있을 경우 
		mp_sListTail->next = pNode ; // 테일의 뒤에 붙임
		mp_sListTail = pNode ; // 테일을 받아온 노드로 지정
	}
}


void CLayerManager::Push(CBaseLayer *pLayer)
{
	if ( m_nTop >= MAX_LAYER_NUMBER ) // 스택이 제일 위로 올라갔을 경우 
	{
#ifdef _DEBUG
		TRACE( "The Stack is full.. so cannot run the push operation.. \n" ) ;
#endif// 디버그 컴파일시 메세지 출력후 리턴
		return ;
	}

	mp_Stack[ ++m_nTop ] = pLayer ; // 스택의 제일 위에 놓음
}

CBaseLayer* CLayerManager::Pop()
{
	if ( m_nTop < 0 ) // 스택이 비었을 경우 
	{
#ifdef _DEBUG
		TRACE( "The Stack is empty.. so cannot run the pop operation.. \n" ) ;
#endif// 디버그 컴파일시 메세지 출력후 리턴
		return NULL ;
	}

	CBaseLayer* pLayer = mp_Stack[ m_nTop ] ; // 스택의 탑을 지정
	mp_Stack[ m_nTop ] = NULL ; // 스택의 탑에 널 지정
	m_nTop-- ; // 탑을 1 낮춤

	return pLayer ; // 탑에 있던 포인터를 반환
}

CBaseLayer* CLayerManager::Top()
{
	if ( m_nTop < 0 ) // 스택이 비어있으면
	{
#ifdef _DEBUG
		TRACE( "The Stack is empty.. so cannot run the top operation.. \n" ) ;
#endif
		return NULL ;
	}

	return mp_Stack[ m_nTop ] ; // 탑 리턴
}

void CLayerManager::LinkLayer(PNODE pNode)
{
	CBaseLayer* pLayer = NULL ;

	while ( pNode ) // 받아온 값이 널이 아니면 
	{
		if ( !pLayer ) // 변수가 널이면 
			pLayer = GetLayer( pNode->token ) ;//받아온 노드의 토큰에 맞는 레이어 받아옴
		else
		{// 널이 아닌경우
			if ( *pNode->token == '(' ) //토큰이 (면 
				Push( pLayer ) ; // 스택에 푸쉬 
			else if ( *pNode->token == ')' ) // 토큰이 ) 면
				Pop( ) ;//팝
			else
			{
				char cMode = *pNode->token ; // 토큰을 변수에 지정
				char* pcName = pNode->token + 1 ; // 토큰의 다음 글자 부터 pcName 에 지정
				
				pLayer = GetLayer( pcName ) ; // 주어진 변수로 레이어 얻어옴

				switch( cMode ) // 모드에 따라서 
				{
				case '*' : Top( )->SetUpperUnderLayer( pLayer ) ; break ; // 상하위 레이어 지정
				case '+' : Top( )->SetUpperLayer( pLayer ) ; break ; // 상위 레이어 지정
				case '-' : Top( )->SetUnderLayer( pLayer ) ; break ; // 하위 레이어 지정
				}
			}
		}

		pNode = pNode->next ; // 다음 리스트로 이동
	}
}

void CLayerManager::DeAllocLayer()
{
	for ( int i = 0 ; i < this->m_nLayerCount ; i ++ ) // 레이어 수 만큼 
		delete this->mp_aLayers[ i ] ; // 레이어 메모리를 반환 한다 
}
