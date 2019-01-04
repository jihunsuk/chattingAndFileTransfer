#if !defined(AFX_STATICROUTERDLG_H__FC102C30_E5DC_4639_9752_4646C9326820__INCLUDED_)
#define AFX_STATICROUTERDLG_H__FC102C30_E5DC_4639_9752_4646C9326820__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "LayerManager.h"
#include "IPLayer.h"
#include "BaseLayer.h"
#include "RoutingTable.h"	

class CStaticRouterDlg : public CDialog{

public:
	void resetList(int flag);
	virtual  ~CStaticRouterDlg();
	CStaticRouterDlg(CWnd* pParent = NULL);	
	// Set 상태 확인
	bool isset;
	CLayerManager m_LayerMgr[MAX_ETHERNET_CARD];
	int ethcnt;
	enum { IDD = IDD_STATICROUTER_DIALOG };
	CListBox	m_lst_ethcard;
	CListCtrl	m_lst_routing;
	CListCtrl	m_lst_proxyarp;
	CListCtrl	m_lst_arpcache;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonSradd();
	afx_msg void OnButtonSrdel();
	afx_msg void OnButtonProxyadd();
	afx_msg void OnButtonArpdel();
	afx_msg void OnButtonProxydel();

	DECLARE_MESSAGE_MAP()

private:
	CRoutingTable m_routingtable;
	LRESULT OnRegUpdatedata(WPARAM wParam, LPARAM lParam);
	CWinThread *RecThr[MAX_ETHERNET_CARD * 2];
	CWinThread *CacheThr;
	bool thrflag[MAX_ETHERNET_CARD];	

public:
	afx_msg void OnLvnItemchangedListRoutingTable(NMHDR *pNMHDR, LRESULT *pResult);
};

typedef struct TPARAM 
{
	CStaticRouterDlg *dlg;
	CIPLayer *iplayer;
	bool *flag1;
	bool *thrflag;
}TPARAM;
#endif 
