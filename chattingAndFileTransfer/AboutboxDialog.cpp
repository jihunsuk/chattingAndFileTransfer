// AboutboxDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ipc.h"
#include "AboutboxDialog.h"
#include "afxdialogex.h"


// AboutboxDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(AboutboxDialog, CDialogEx)

AboutboxDialog::AboutboxDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ABOUTBOX, pParent)
{

}

AboutboxDialog::~AboutboxDialog()
{
}

void AboutboxDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AboutboxDialog, CDialogEx)
END_MESSAGE_MAP()


// AboutboxDialog �޽��� ó�����Դϴ�.
