// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__97512FEA_412C_4E05_B2A4_DADC2CEA5A0F__INCLUDED_)
#define AFX_STDAFX_H__97512FEA_412C_4E05_B2A4_DADC2CEA5A0F__INCLUDED_

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
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#define MAX_ETHERNET_CARD 5
#define MAX_ENTRY 255

#define MAX_LAYER_NUMBER	0xff	/* 상위 레이어 수 더이상 의미 없음 */
#define MAX_CHAT_DATA 1456			/* Chat Data size */
#define CHAT_HSIZE 2 + 1 + 1		/* Chat Header size */
//#define MAX_FILE_DATA 1448			/* File Data size */
//#define FILE_HSIZE 12				/* File Header size */
#define MAX_ETHERNET_DATA 1500		/* Ethernet Data size */
#define ETH_HSIZE 14//6 + 6 + 2			/* Ethernet Header size */
#define ARP_HSIZE 28
#define IP_HSIZE 20
#define MAX_IP_DATA 1480

#define ETHERTYPE_PUP		0x0200	/* Xerox PUP */
#define ETHERTYPE_IP		0x0800	/* IP */
#define ETHERTYPE_ARP		0x0806	/* Address resolution */
#define ETHERTYPE_REVARP	0x8035	/* Reverse ARP */
#define ETHERTYPE_CHAT		0x0830	/* Chat APP */
#define ARP_HTYPE_ETH		0x0001	// arp hardware type 1
#define ARP_HSIZE_ETH		0x06	
#define ARP_PSIZE_IP		0x04
#define ARP_OP_REQUEST		0x0001	
#define ARP_OP_REPLY		0x0002	
#define RARP_OP_REQUEST		0x0003	
#define RARP_OP_REPLY		0x0004	
#define ARP_GRTS			0x0005

#define DATA_TYPE_CONT		0x01	/* DATA_TYPE_CONT */
#define DATA_TYPE_END		0x02	/* DATA_TYPE_END */
#define DATA_TYPE_ACK		0x10	/* DATA_TYPE_ACK reply */
/*#define FILE_TYPE_SEND		0x20	// CAN I SEND FILE 
#define FILE_TYPE_SENDOK	0x21	// OK 
#define FILE_TYPE_CANCEL	0xFF	// send, recv Cancle */

typedef struct _CHAT_Protocol
{
	unsigned short capp_totlen;// 메시지총길이
	unsigned char capp_type;// 메시지타입
	unsigned char capp_unused;// 우선사용안함
	unsigned char capp_data[ MAX_CHAT_DATA ] ;
} CHAT_pro, *LPCHAT_pro ;	// 챗 프로토콜 
/*
typedef struct _FILE_protocol
{
	unsigned long fapp_totlen;// 총길이
	unsigned short fapp_type;// 데이터타입
	unsigned char faa_msg_type;// 메시지종류
	unsigned char ed;// 사용안함
	unsigned long fapp_seq_num;// fragmentation 순서
	unsigned char fapp_data[ MAX_FILE_DATA ] ;
} FILE_pro, *LPFILE_pro ;	// 파일 프로토콜 */

typedef struct _ETHERNET_ADDR
{
	unsigned char s_ether_addr[6];
} ETHERNET_ADDR, *LPETHERNET_ADDR ; // 이더넷 어드레스를 위한 구조체 (MAC)

typedef struct _IP_ADDR 
{
	unsigned char s_ip_addr[4];
} IP_ADDR, *LPIP_ADDR;

typedef struct _ETHERNET
{
	ETHERNET_ADDR enet_desaddr;	// 상대방주소
	ETHERNET_ADDR enet_srcaddr;	// 자기주소
	unsigned short enet_type;	// frame data type
	unsigned char enet_data[ MAX_ETHERNET_DATA ] ;
} ETHERNET, *LPETHERNET ;	// 이더넷 프로토콜?

typedef struct _ARP_protocol
{
	unsigned short	hard_type;	// hardware type
	unsigned short	prot_type;	// protocol type
	unsigned char	hard_size;	// eth addr size 6;
	unsigned char	prot_size;	// ip addr size 4;
	unsigned short	op;	// arp op
	ETHERNET_ADDR	enet_srcaddr;	// sender eth 주소
	IP_ADDR			ip_srcaddr;	// sender ip 주소
	ETHERNET_ADDR	enet_desaddr;	// target eth 주소
	IP_ADDR			ip_desaddr; // target ip 주소
} ARP, *LPARP ;	// ARP 프로토콜?

typedef struct _IP_protocol
{
	unsigned char version_headerlength;	// 버전과 헤더 길이
	unsigned char TOS;		// type of service 무시
	unsigned short totlen;	// 총길이
	unsigned short ident;	// 무시
	unsigned short flag_frag;	// 조각여부, 조각 위치 무시
	unsigned char TTL;		// 몇개의 라우터를 거쳐갈수 있는지 무시
	unsigned char prot;		// 상위레이어의 타입 무시
	unsigned short checksum;// 무시
	IP_ADDR ip_srcaddr;	// 자기주소
	IP_ADDR ip_desaddr;	// 상대방주소
	unsigned char ip_data[ MAX_IP_DATA ] ;
} IP, *LPIP ;	// IP 프로토콜?

typedef struct _ALL_flag
{
	unsigned short app_len;		//이번 길이
	unsigned short app_type;
	IP_ADDR desip;
	int ethsel;
} ALL_flag, *LPALL_flag; // 레이어에서 필요한 각종 플래그 

#endif // !defined(AFX_STDAFX_H__97512FEA_412C_4E05_B2A4_DADC2CEA5A0F__INCLUDED_)
