// TCPLayer.h: interface for the TCPLayer class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"


class CIPLayer
	: public CBaseLayer
{
private:
	inline void		ResetHeader();

public:
	unsigned char*		Receive();
	int			Send(unsigned char* ppayload, int nlength);
	void			SetDestinAddress(unsigned char* pAddress);
	void			SetSourceAddress(unsigned char* pAddress);
	unsigned char*	GetDestinAddress();
	unsigned char*	GetSourceAddress();

	CIPLayer(char* pName);
	virtual ~CIPLayer();

	typedef struct _IP_HEADER {

		unsigned char	ip_dstaddr[4];		// destination address of ethernet layer
		unsigned char	ip_srcaddr[4];		// source address of ethernet layer
		unsigned short	ip_type;		// type of ethernet layer
		unsigned char	ip_data[IP_DATA_SIZE]; // frame data

	} IP_HEADER, *PIP_HEADER;

protected:
	IP_HEADER	m_sHeader;
};