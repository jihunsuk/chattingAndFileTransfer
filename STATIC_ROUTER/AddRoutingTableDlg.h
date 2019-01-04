#if !defined(AFX_ADDROUTINGTABLEDLG_H__D25BEFDB_A1FC_4F7A_B607_A53462E411AA__INCLUDED_)
#define AFX_ADDROUTINGTABLEDLG_H__D25BEFDB_A1FC_4F7A_B607_A53462E411AA__INCLUDED_

#include "RoutingTable.h"	
#include "LayerManager.h"	
#include "StdAfx.h"
#if _MSC_VER > 1000
#pragma once
#endif 

class CAddRoutingTableDlg : public CDialog{

public:
	IP_ADDR Andip(IP_ADDR &ip1, IP_ADDR &ip2);
	CRoutingTable *mp_rtable;
	void setData(CLayerManager *lmng, int cnt, CRoutingTable *rtable);
	CLayerManager *mp_layermng;
	CAddRoutingTableDlg(CWnd* pParent = NULL);  
	int ethcnt;
	enum { IDD = IDD_ADDROUTINGTABLEDIALOG };
	CIPAddressCtrl	m_ip_netmask;
	CIPAddressCtrl	m_ip_gate;
	CIPAddressCtrl	m_ip_des;
	CComboBox	m_cbo_eth;
	BOOL	m_chk_gate;
	BOOL	m_chk_host;
	BOOL	m_chk_up;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnCheckGate();
	afx_msg void OnCheckHost();
	DECLARE_MESSAGE_MAP()
};

#endif 
