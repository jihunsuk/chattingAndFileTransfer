; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CStaticRouterDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "StaticRouter.h"

ClassCount=5
Class1=CStaticRouterApp
Class2=CStaticRouterDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_ADDROUTINGTABLEDIALOG
Class4=CAddRoutingTableDlg
Resource4=IDD_STATICROUTER_DIALOG
Class5=CAddProxyARPDlg
Resource5=IDD_ADDPROXYDIALOG

[CLS:CStaticRouterApp]
Type=0
HeaderFile=StaticRouter.h
ImplementationFile=StaticRouter.cpp
Filter=N

[CLS:CStaticRouterDlg]
Type=0
HeaderFile=StaticRouterDlg.h
ImplementationFile=StaticRouterDlg.cpp
Filter=D
LastObject=ID_BUTTON_SRADD
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=StaticRouterDlg.h
ImplementationFile=StaticRouterDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_STATICROUTER_DIALOG]
Type=1
Class=CStaticRouterDlg
ControlCount=12
Control1=ID_BUTTON_SRADD,button,1342242816
Control2=IDC_BUTTON_SRDEL,button,1342242816
Control3=IDC_LIST_ROUTING_TABLE,SysListView32,1350664197
Control4=IDC_STATIC,button,1342177287
Control5=IDC_STATIC,button,1342177287
Control6=ID_BUTTON_ARPDEL,button,1342242816
Control7=IDC_STATIC,button,1342177287
Control8=ID_BUTTON_PROXYDEL,button,1342242816
Control9=IDC_LIST_ARPCACHE,SysListView32,1350664197
Control10=IDC_LIST_PROXYARP,SysListView32,1350664197
Control11=ID_BUTTON_PROXYADD,button,1342242816
Control12=IDC_LIST_ETHCARD,listbox,1350648065

[DLG:IDD_ADDROUTINGTABLEDIALOG]
Type=1
Class=CAddRoutingTableDlg
ControlCount=14
Control1=IDOK,button,1342242816
Control2=IDCANCEL,button,1342242816
Control3=IDC_IP_DES,SysIPAddress32,1342242816
Control4=IDC_IP_NETMASK,SysIPAddress32,1342242816
Control5=IDC_IP_GATE,SysIPAddress32,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,button,1342177287
Control10=IDC_CHECK_UP,button,1476460547
Control11=IDC_CHECK_GATE,button,1342242819
Control12=IDC_CHECK_HOST,button,1342242819
Control13=IDC_COMBO_ETH,combobox,1344339970
Control14=IDC_STATIC,static,1342308864

[CLS:CAddRoutingTableDlg]
Type=0
HeaderFile=AddRoutingTableDlg.h
ImplementationFile=AddRoutingTableDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CAddRoutingTableDlg

[DLG:IDD_ADDPROXYDIALOG]
Type=1
Class=CAddProxyARPDlg
ControlCount=8
Control1=IDOK,button,1342242816
Control2=IDCANCEL,button,1342242816
Control3=IDC_COMBO_ETHCARD,combobox,1344339970
Control4=IDC_IPADDRESS,SysIPAddress32,1342242816
Control5=IDC_EDIT_MAC,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[CLS:CAddProxyARPDlg]
Type=0
HeaderFile=AddProxyARPDlg.h
ImplementationFile=AddProxyARPDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDOK
VirtualFilter=dWC

