#pragma once


// AboutboxDialog ��ȭ �����Դϴ�.

class AboutboxDialog : public CDialogEx
{
	DECLARE_DYNAMIC(AboutboxDialog)

public:
	AboutboxDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~AboutboxDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
