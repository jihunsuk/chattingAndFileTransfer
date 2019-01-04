// ARPCache.h: interface for the CARPCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARPCACHE_H__DDD69FF7_BEC8_4DDE_AA60_C07E63892320__INCLUDED_)
#define AFX_ARPCACHE_H__DDD69FF7_BEC8_4DDE_AA60_C07E63892320__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct ARP_CACHE_ITEM
{
	IP_ADDR IPaddr;
	ETHERNET_ADDR ETHaddr;
	BOOL success;
	time_t regtime;
	time_t remindtime;
	int ethcard;
}ARP_CACHE_ITEM;

typedef struct PROXY_ARP_ITEM 
{
	int ethcard;
	IP_ADDR IPaddr;
	ETHERNET_ADDR ETHaddr;
}PROXY_ARP_ITEM;

class CARPCache  
{
public:
	CARPCache();
	virtual ~CARPCache();
	bool setARPCache(ARP_CACHE_ITEM &item);
	bool setProxy(PROXY_ARP_ITEM &item);
	bool setProxy(ETHERNET_ADDR &ethaddr, IP_ADDR &ipaddr);
	ARP_CACHE_ITEM* getARPCache(IP_ADDR &ipaddr);
	PROXY_ARP_ITEM* getProxy(IP_ADDR &ipaddr);
	ARP_CACHE_ITEM* getARPCache(int &index);
	PROXY_ARP_ITEM* getProxy(int &index);
	int getARPcnt();
	int getProxycnt();
	void delARPcache(int &index);
	void delProxycache(int &index);

private:
	BOOL Comip(IP_ADDR &ip1, IP_ADDR &ip2);

	static int arpcnt;
	static ARP_CACHE_ITEM m_arp_cache[MAX_ENTRY];
	static int proxycnt;
	static PROXY_ARP_ITEM m_proxy_arp[MAX_ENTRY];
};

#endif // !defined(AFX_ARPCACHE_H__DDD69FF7_BEC8_4DDE_AA60_C07E63892320__INCLUDED_)
