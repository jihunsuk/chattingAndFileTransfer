// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#include <SDKDDKVer.h>
#if !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#define AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}

#define MAX_LAYER_NUMBER		0xff

#define ETHER_MAX_SIZE			1514//+300
#define ETHER_HEADER_SIZE		14
#define ETHER_MAX_DATA_SIZE		( ETHER_MAX_SIZE - ETHER_HEADER_SIZE )

#define IP_HEADER_SIZE			10
#define IP_DATA_SIZE			1490

#define TCP_HEADER_SIZE			6
#define TCP_DATA_SIZE			1484

#define CHAT_REC				0x01//used APP_DATA_MSG (REC)
#define CHAT_NOTREC				0x02//used APP_DATA_MSG (NOTREC)

#define	CHAT_TYPE				0x2080
#define	FILE_TYPE				0x2090

//#define DATA_HEAD				0x00
//#define DATA_MAIN				0x01
//#define DATA_LAST				0x02

//4 byte
#define APP_HEADER_SIZE			( sizeof(unsigned char)  +		    		\
								  sizeof(unsigned short) +					\
								  sizeof(unsigned char)	)
#define APP_DATA_SIZE			( ETHER_MAX_DATA_SIZE - ( APP_HEADER_SIZE +		\
												          TCP_HEADER_SIZE +		\
												          IP_HEADER_SIZE ) )

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__119ECB1B_6E70_4662_A2A9_A20B5201CA81__INCLUDED_)
#include <afxcontrolbars.h>
