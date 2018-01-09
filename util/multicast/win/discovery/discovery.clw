; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CDiscoveryDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "discovery.h"

ClassCount=2
Class1=CDiscoveryApp
Class2=CDiscoveryDlg

ResourceCount=3
Resource2=IDR_MAINFRAME
Resource3=IDD_DISCOVERY_DIALOG

[CLS:CDiscoveryApp]
Type=0
HeaderFile=discovery.h
ImplementationFile=discovery.cpp
Filter=N

[CLS:CDiscoveryDlg]
Type=0
HeaderFile=discoveryDlg.h
ImplementationFile=discoveryDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_DEV_IP_SHOW



[DLG:IDD_DISCOVERY_DIALOG]
Type=1
Class=CDiscoveryDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_DEV_IP_SHOW,static,1342308352
Control5=IDC_SEARCH,button,1342242816
Control6=IDC_IPADDRESS,SysIPAddress32,1342242816

