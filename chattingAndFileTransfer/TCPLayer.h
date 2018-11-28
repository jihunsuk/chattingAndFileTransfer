// TCPLayer.h: interface for the TCPLayer class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"


class CTCPLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();

public:
	unsigned char*			Receive(int Thread_type);
	int				Send(unsigned char* ppayload, int nlength,int port);
	void			SetDestinAddress(unsigned char* pAddress);
	void			SetSourceAddress(unsigned char* pAddress);
	unsigned char*	GetDestinAddress();
	unsigned char*	GetSourceAddress();

	CTCPLayer(char* pName);
	virtual ~CTCPLayer();

	typedef struct _TCP_HEADER {

		unsigned char	tcp_dstaddr[2];		// destination address of ethernet layer
		unsigned char	tcp_srcaddr[2];		// source address of ethernet layer
		unsigned short	tcp_type;		// type of ethernet layer
		unsigned char	tcp_data[TCP_DATA_SIZE]; // frame data

	} TCP_HEADER, *PTCP_HEADER;
	
	BOOL _Rock;
	unsigned char chat_Packet[IP_DATA_SIZE];//C
	unsigned char file_Packet[IP_DATA_SIZE];//C
protected:
	TCP_HEADER	m_sHeader;
};