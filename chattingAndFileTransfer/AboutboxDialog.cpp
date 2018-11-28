// AboutboxDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ipc.h"
#include "AboutboxDialog.h"
#include "afxdialogex.h"


// AboutboxDialog 대화 상자입니다.

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


// AboutboxDialog 메시지 처리기입니다.
