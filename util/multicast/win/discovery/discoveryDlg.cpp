// discoveryDlg.cpp : implementation file
//
#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include "discovery.h"
#include "discoveryDlg.h"
#include "common.h"
#include <winsock2.h>
//#include <ws2tcpip.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "wsock32.lib") 
#pragma comment(lib, "ws2_32.lib")	//winsock 2.0 

#define MAX_RETRY_NUM	3			//组播发现设备时，重复发送组播包数

/////////////////////////////////////////////////////////////////////////////
// CDiscoveryDlg dialog

CDiscoveryDlg::CDiscoveryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiscoveryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiscoveryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDiscoveryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiscoveryDlg)
	DDX_Control(pDX, IDC_DEV_IP_SHOW, m_devIpShow);
	DDX_Control(pDX, IDC_IPADDRESS, m_ip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDiscoveryDlg, CDialog)
	//{{AFX_MSG_MAP(CDiscoveryDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiscoveryDlg message handlers

BOOL CDiscoveryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDiscoveryDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDiscoveryDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CDiscoveryDlg::OnSearch() 
{
	// TODO: Add your control notification handler code here
	// TODO: 在此添加控件通知处理程序代码
	WSADATA wsd;
	struct sockaddr_in local,remote,from;
	SOCKET sock,sockM;
	char sendbuf[MAX_BUF_SIZE];
	board_info_t stDeviceInfo;
	DWORD dwIP; 
	CString devIpShow = "";

	/*struct ip_mreq mcast; // Winsock1.0 */
	int len = sizeof( struct sockaddr_in);
	int ret;
	int i = 0, j = 0, k = 0, n;
	fd_set set;
	struct timeval timeout;

	//初始化WinSock2.2
	if( WSAStartup(MAKEWORD(2,2),&wsd) != 0 )
	{
		AfxMessageBox(_T("WSAStartup() failed"));
		return;
	}

	m_ip.GetAddress(dwIP); 
	/*
	创建一个SOCK_DGRAM类型的SOCKET
	*/
	if ((sock= WSASocket(AF_INET,SOCK_DGRAM,0,NULL,0,
		                 WSA_FLAG_MULTIPOINT_C_LEAF|WSA_FLAG_MULTIPOINT_D_LEAF|
		                 WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("socket failed with:%d\n",WSAGetLastError());
		WSACleanup();
		return;
	}

	//将sock绑定到本机某端口上。
	local.sin_family = AF_INET;
	local.sin_port = htons(MULTICAST_PORT);
	local.sin_addr.s_addr = htonl(dwIP)/*INADDR_ANY*/;
	if(bind(sock,(struct sockaddr *)&local,sizeof(local)) == SOCKET_ERROR)
	{
		printf( "bind failed with:%d \n",WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return;
	}

	//加入多播组
	remote.sin_family = AF_INET;
	remote.sin_port = htons(MULTICAST_PORT);
	remote.sin_addr.s_addr = inet_addr( MULTICAST_ADDR );

	/* Winsock2.0*/
	if(( sockM = WSAJoinLeaf(sock,(SOCKADDR *)&remote,sizeof(remote),NULL,NULL,NULL,NULL,
		JL_BOTH)) == INVALID_SOCKET)
	{
		printf("WSAJoinLeaf() failed:%d\n",WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return;
	}

	strcpy(sendbuf, MAGIC_STR);

#if 0
	for(i = 0; i < MAX_RETRY_NUM; i++) 
	{
		if (SOCKET_ERROR == sendto(sock, sendbuf, (int)strlen(sendbuf), 0, (struct sockaddr*)&remote, sizeof(remote))){
			printf("sendto failed with: %d\n",WSAGetLastError());
			closesocket(sockM);
			closesocket(sock);
			WSACleanup();
			return;
		}
	}
#endif 
	//连续3次发送组播包
	for (i = 0; i < MAX_RETRY_NUM; i++)
	{
		if (SOCKET_ERROR == sendto(sock, sendbuf, (int)strlen(sendbuf), 0, (struct sockaddr*)&remote, sizeof(remote)))
		{
			printf("sendto failed with: %d\n",WSAGetLastError());
			closesocket(sockM);
			closesocket(sock);
			WSACleanup();
			return;
		}
		
		j = 0;
		while(1) 
		{
			FD_ZERO(&set);		//初始化监视列表
			FD_SET(sock, &set);	//将所要监视的设备加入列表
		
			/* 设置时间 */
			timeout.tv_sec = 2;	
			timeout.tv_usec = 0;	
			
			printf("select %d: %d\n", i, j);
			if ((n = select(FD_SETSIZE, &set, NULL, NULL, &timeout)) < 0) 
			{
				/* 异常中断，则继续进行当前时间 */
				if(errno == -1) //EINTR) 
				{
					printf("EINTR\n");
					continue;
				}
			}
			
			if(n == 0) 
			{
				break;
			}
	
			memset(&stDeviceInfo, 0, sizeof(board_info_t));
			if (SOCKET_ERROR == ( ret = recvfrom(sock, (char *)&stDeviceInfo, sizeof(board_info_t), 0, (struct sockaddr *)&from, &len))) {
				printf("recvfrom failed with:%d\n",WSAGetLastError());
				closesocket(sockM);
				closesocket(sock);
				WSACleanup();
				return ;
			}

			/* 检验接受到的数据是否有效 */
			if (stDeviceInfo.flag != MAGIC_FLAG) 
			{
				printf("Flag error\n");
				continue;
			}
		
			//maybe alloc a array to store boards info
			devIpShow.Empty();

			devIpShow += "dev ip:";
			devIpShow += stDeviceInfo.ip;
			devIpShow += "\n name: "; 
			devIpShow += stDeviceInfo.name;
			devIpShow += "\n type:";
			devIpShow += stDeviceInfo.type;
			devIpShow += "\n soft ver:";
			devIpShow += stDeviceInfo.soft_ver;

			m_devIpShow.SetWindowText(devIpShow);  

			j++;
		}		
	}

	closesocket(sockM);
	closesocket(sock);
	WSACleanup();

	return ;	
}
