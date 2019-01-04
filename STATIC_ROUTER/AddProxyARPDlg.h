#if !defined(AFX_ADDPROXYARPDLG_H__46380073_BB05_4D5B_98C7_6C38F376D800__INCLUDED_)
#define AFX_ADDPROXYARPDLG_H__46380073_BB05_4D5B_98C7_6C38F376D800__INCLUDED_

#include "LayerManager.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddProxyARPDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddProxyARPDlg dialog

class CAddProxyARPDlg : public CDialog
{
// Construction
public:
	void CharToHex(UCHAR* mac, CString str);
	void setData(CLayerManager *lmng, int cnt);
	int ethcnt;
	CAddProxyARPDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddProxyARPDlg)
	enum { IDD = IDD_ADDPROXYDIALOG };
	CIPAddressCtrl	m_ip_ip;
	CComboBox	m_cbo_ethcard;
	CString	m_str_mac;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddProxyARPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddProxyARPDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CLayerManager *mp_layermng;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDPROXYARPDLG_H__46380073_BB05_4D5B_98C7_6C38F376D800__INCLUDED_)
