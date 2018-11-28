// FileLayer.h: interface for the CFileLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
#define AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

#define DATA_HEAD				0x00//call to setting
#define DATA_MAIN				0x01//default data
#define DATA_LAST				0x02//last data

#define MSG_DATA				0x00//all of MSG in data
#define MSG_RESP				0x01//respond(setting)
#define MSG_ERR					0x02//err MSG(receive position)
#define MSG_COR					0x03//err correct MSG(send position)
#define MSG_LASTOK				0x04
#define MSG_CANCLE				0x05//cancle

#define FILE_INFO_SIZE			( sizeof(unsigned char) * 255 +				\
	sizeof(unsigned long))
//12byte
#define FILE_HEADER_SIZE		( sizeof(unsigned long)*2 +					\
	sizeof(unsigned short) +\
	sizeof(unsigned char) * 2)
#define FILE_DATA_SIZE			( ETHER_MAX_DATA_SIZE - ( FILE_HEADER_SIZE +		\
	TCP_HEADER_SIZE + \
	IP_HEADER_SIZE))

class CFileLayer
	: public CBaseLayer
{
public:
	int errReady;//if error occured setting value 0 -> 1
	BOOL BooleanErr;//ack Sucess and transport packet

	BOOL RecEnd;//Resend packet (this used 'time-out' value

	unsigned int totalLength;
	unsigned long check;
	typedef struct _FILE_APP_INFO {
		unsigned long totalLength;
		unsigned char fileName[255];
	} FILE_APP_INFO, *PFILE_APP_INFO;

	typedef struct _FILE_APP {
		unsigned long fapp_totlen;		// file length
		unsigned short fapp_type;		// file data tyte
		unsigned char fapp_msg_type;	// file message type
		unsigned char ed;				// don't used
		unsigned long fapp_seq_num;		// seq_num
		unsigned char fapp_data[FILE_DATA_SIZE];	// file_data size
	} FILE_APP, *PFILE_APP;

	unsigned char*	Receive(int Thread_type);
	BOOL	Send(unsigned char* ppayload, int nlength, int port);
	void	ResetHeader();

	CFileLayer(char* pName);
	virtual ~CFileLayer();

	CString recvPath;//file path
	BOOL m_ackReady;
	CFile m_recvFile;
	FILE_APP m_sHeader;
	unsigned char filebuf[FILE_DATA_SIZE];

//protected:
//	afx_msg void OnTimer(UINT nIDEvent); // (this used 'time-out' value
};



#endif // !defined(AFX_FILELAYER_H__D67222B3_1B00_4C77_84A4_CEF6D572E181__INCLUDED_)
