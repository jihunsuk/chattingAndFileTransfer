// ARPCache.cpp: implementation of the CARPCache class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StaticRouter.h"
#include "ARPCache.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//½ºÅÂÆ½ º¯¼ö 
int CARPCache::proxycnt = 0;
PROXY_ARP_ITEM CARPCache::m_proxy_arp[MAX_ENTRY];
int CARPCache::arpcnt = 0;
ARP_CACHE_ITEM CARPCache::m_arp_cache[MAX_ENTRY];

CARPCache::CARPCache()
{

}

CARPCache::~CARPCache()
{
	
}

bool CARPCache::setARPCache(ARP_CACHE_ITEM &item)
{
	ARP_CACHE_ITEM* temp;
	if(temp = getARPCache(item.IPaddr))
	{
		*temp = item;
		return true;
	}
	if(arpcnt < MAX_ENTRY)
	{
		m_arp_cache[arpcnt++] = item;
		return true;
	}
	return false;
}

bool CARPCache::setProxy(PROXY_ARP_ITEM &item)
{
	PROXY_ARP_ITEM* temp;
	if(temp = getProxy(item.IPaddr))
	{
		*temp = item;
		return true;
	}
	if(proxycnt < MAX_ENTRY)
	{
		m_proxy_arp[proxycnt++] = item;
		return true;
	}
	return false;
}

bool CARPCache::setProxy(ETHERNET_ADDR &ethaddr, IP_ADDR &ipaddr)
{
	if(proxycnt < MAX_ENTRY)
	{
		m_proxy_arp[proxycnt].ETHaddr = ethaddr;
		m_proxy_arp[proxycnt++].IPaddr = ipaddr;
		return true;
	}
	return false;
}

ARP_CACHE_ITEM* CARPCache::getARPCache(IP_ADDR &ipaddr)
{
	int i;
	for(i = 0; i < arpcnt; i++) 
	{
		if(Comip(m_arp_cache[i].IPaddr, ipaddr))
			return &m_arp_cache[i];
	}
	return NULL;
}

PROXY_ARP_ITEM* CARPCache::getProxy(IP_ADDR &ipaddr)
{
	int i;
	for(i = 0; i < proxycnt; i++) 
	{
		if(Comip(m_proxy_arp[i].IPaddr, ipaddr))
			return &m_proxy_arp[i];
	}
	return NULL;
}

ARP_CACHE_ITEM* CARPCache::getARPCache(int &index)
{
	return &m_arp_cache[index];
}

PROXY_ARP_ITEM* CARPCache::getProxy(int &index)
{
	return &m_proxy_arp[index];
}

int CARPCache::getARPcnt()
{
	return arpcnt;
}

int CARPCache::getProxycnt()
{
	return proxycnt;
}

void CARPCache::delARPcache(int &index)
{
	int i = 0;
	for(i = index; i < arpcnt; i++)
	{
		m_arp_cache[i] = m_arp_cache[i+1];
	}
	arpcnt--;
}

void CARPCache::delProxycache(int &index)
{
	int i = 0;
	for(i = index; i < proxycnt; i++)
	{
		m_proxy_arp[i] = m_proxy_arp[i+1];
	}
	proxycnt--;
}

BOOL CARPCache::Comip(IP_ADDR &ip1, IP_ADDR &ip2)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		if(ip1.s_ip_addr[i] != ip2.s_ip_addr[i])
			return false;
	}
	return true;
}
