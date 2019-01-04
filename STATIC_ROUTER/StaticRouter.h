#if !defined(AFX_STATICROUTER_H__BD42A08F_4E18_43E8_AE8A_9CD5D4DAD33F__INCLUDED_)
#define AFX_STATICROUTER_H__BD42A08F_4E18_43E8_AE8A_9CD5D4DAD33F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

class CStaticRouterApp : public CWinApp{
public:
	CStaticRouterApp();
	public:
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};

#endif 
