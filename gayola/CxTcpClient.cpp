
#include "CxTcpClient.h"



namespace xs
{


	int CxTcpClient::Startup()
	{

#ifdef _MSC_VER

		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			wprintf(L"WSAStartup function failed with error: %d\n", iResult);
			return 1;
		}
#else

#endif

		return 0;
	}

	void CxTcpClient::Shutdown()
	{
#ifdef _MSC_VER
		WSACleanup();
#endif
	}

	void CxTcpClient::SetSocketNoBlock(XSOCKET fd,bool nonblock)
	{
		
#ifdef _MSC_VER
		unsigned long ul = 0;
		if (nonblock) {
			ul = 0;
			ioctlsocket(fd, FIONBIO, &ul); 
		}
		else {
			ul = 1;
			ioctlsocket(fd, FIONBIO, &ul);
		}
#else

		int flags;

		flags = fcntl(fd, F_GETFL, 0);
		if (flags < 0)
		{
			//i_fatal("fcntl(%d, F_GETFL) failed: %m", fd);
		}

		if (nonblock)
			flags |= O_NONBLOCK;
		else
			flags &= ~O_NONBLOCK;

		if (fcntl(fd, F_SETFL, flags) < 0) {
		//	i_fatal("fcntl(%d, F_SETFL) failed: %m", fd);
		}
#endif
	}

	static CxTcpClient* _shared = NULL;

	CxTcpClient* CxTcpClient::shared()
	{
		if (_shared == NULL) _shared = new CxTcpClient();
		return _shared;
	}

	void CxTcpClient::FreeShared()
	{
		if (_shared) {
			delete _shared;
			_shared = NULL;
		}
	}

	CxTcpClient::CxTcpClient()
	{
		ConnectSocket=0;
		m_nBreakHeart = 10;
		Clear();
	}

	CxTcpClient::~CxTcpClient()
	{
		Disconnect();
	}

#ifndef _MSC_VER
	int CxTcpClient::Connect(std::string ipAddr, int port,int tio)
	{
		struct sockaddr_in server_addr;
		socklen_t server_addr_length = sizeof(server_addr);
		timeval tm={0,tio};
		int rc=0;
		struct sockaddr_in client_addr;

		m_ipSvr=ipAddr;
		m_portSvr=port;

		bzero(&client_addr, sizeof(client_addr)); //把一段内存区的内容全部设置为0
		client_addr.sin_family = AF_INET;    //internet协议族
		client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY表示自动获取本机地址
		client_addr.sin_port = htons(0);    //0表示让系统自动分配一个空闲端口
											//创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
		ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (ConnectSocket < 0)
		{
			printf("Create Socket Failed!\n");
			rc=-1;
			goto cc2;
			//		exit(1);
		}
		//把客户机的socket和客户机的socket地址结构联系起来
		if (bind(ConnectSocket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
		{
			printf("Client Bind Port Failed!\n");
			rc=-1;
			goto cc2;
		}

		//设置一个socket地址结构server_addr,代表服务器的internet地址, 端口

		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		if (inet_aton(ipAddr.c_str(), &server_addr.sin_addr) == 0) //服务器的IP地址来自程序的参数
		{
			printf("Server IP Address Error!\n");
			rc=-2;
			goto cc2;
		}
		server_addr.sin_port = htons(port);

		//向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接

	#if(0)
		if (connect(ConnectSocket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
		{
			printf("Can Not Connect To %s : %d!\n", ipAddr.c_str(),port);
			//		exit(1);
			return 0;
		}
	#else
		SetSocketNoBlock(ConnectSocket, true);

		if (connect(ConnectSocket, (struct sockaddr *)&server_addr, server_addr_length) < 0)
		{
			int error = -1;
			socklen_t len = sizeof(socklen_t);
			fd_set set;
			FD_ZERO(&set);
			FD_SET(ConnectSocket, &set);
			if (select(ConnectSocket+1, NULL, &set, NULL, &tm) > 0)
			{
				getsockopt(ConnectSocket, SOL_SOCKET, SO_ERROR, (void*)&error, (socklen_t*)&len);
				if (error != 0) {
					rc=error;
					goto cc2;
				}
			}
			else{
				rc=-1;
				goto cc2;
			}
		}

		SetSocketNoBlock(ConnectSocket, false);
		rc=0;
	#endif
		cc2:
		OnConnectAfter(rc);
		return rc;
	}

#else
	int CxTcpClient::Connect(std::string ipAddr, int port,int tio)
	{
		int iResult = 0;
		timeval tm;
		fd_set set;
		int error = -1;
		int len = sizeof(int);

		DispatchEvent(XTCS_CONNECT,0 , 0);

		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET) {
			wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
			return -1;
		}

		//----------------------
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(ipAddr.c_str());
		clientService.sin_port = htons(port);


#if(0)
		iResult = connect(ConnectSocket, (SOCKADDR *)& clientService, sizeof(clientService));
		if (iResult == SOCKET_ERROR) {
			wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
			iResult = closesocket(ConnectSocket);
			if (iResult == SOCKET_ERROR)
				wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			//		WSACleanup();
			return 1;
		}
#endif

		SetSocketNoBlock(ConnectSocket, true);

		bool ret = true;
		if (connect(ConnectSocket, (struct sockaddr *)&clientService, sizeof(clientService)) == -1)
		{
			ret = false;
			tm.tv_sec = tio; 
			tm.tv_usec = 0;
			FD_ZERO(&set);
			FD_SET(ConnectSocket, &set);
			if (select(ConnectSocket + 1, NULL, &set, NULL, &tm) > 0)
			{
				getsockopt(ConnectSocket, SOL_SOCKET, SO_ERROR, (char *)&error, /*(socklen_t *)*/&len);
				ret = (error == 0) ? true : false;
			}
		}

		SetSocketNoBlock(ConnectSocket, true);
		OnConnectAfter(ret?0:-1);

		return ret;
	}
#endif


	int CxTcpClient::Disconnect()
	{
		m_bConnect = false;

		if (ConnectSocket < 1) {
			goto cc2;
		}

#ifdef _MSC_VER
		int iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR) {
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
			//		WSACleanup();
			return 1;
		}
#else
		close(ConnectSocket);
#endif

	cc2:
		OnConnectAfter(0);
		return 0;
	}

	void CxTcpClient::RecvIm(char* buffer, int sz)
	{

	}

	void CxTcpClient::Step()
	{
		if (!m_bConnect) return;

		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);

		FD_SET(ConnectSocket, &readfds);
		FD_SET(ConnectSocket, &writefds);
		FD_SET(ConnectSocket, &exceptfds);

		struct timeval tv ={0,100};
		int retval=0;

		retval = select(ConnectSocket+1, &readfds, &writefds, &exceptfds, &tv);
		if (retval > 0)
		{
			if (FD_ISSET(ConnectSocket, &readfds))
			{
				int rc = ::recv(ConnectSocket, m_buffer,sizeof(m_buffer)-1, 0);
				if (rc > 0) {
					m_buffer[rc] = 0;
					for (auto it : m_lis)
					{
						(it)->OnTcpClientDataRecv(this, m_buffer, rc);
					}
				}

				if (rc < 0)
				{
					printf("+++++++connect disconnect++++\n");
					Disconnect();
				}

			}

			if (FD_ISSET(ConnectSocket, &writefds))
			{
			}

			if (FD_ISSET(ConnectSocket, &exceptfds))
			{
				for (auto it : m_lis)
				{
					(it)->OnTcpClientError(this, GetLastErrorCode());
				}
			}


		}

		//发送数据
		std::lock_guard<std::mutex> lck(m_mtx_send_que);
		while (!m_send_que.empty())
		{
			CxDatChunk* dc = m_send_que.front();
			m_send_que.pop_front();
			SendDataIme(dc->c_str(), dc->size());
			delete dc;
		}

		//看是否发送心跳包
		if (m_nBreakHeart > 0 && difftime(time(NULL), m_tiLastSend) >= m_nBreakHeart)
		{
			SendBreakHeart();
		}

	}

	void CxTcpClient::SendData(const void* buf, int sz)
	{
		std::lock_guard<std::mutex> lck(m_mtx_send_que);
		CxDatChunk* dc = new CxDatChunk();
		dc->SetData(buf, sz);
		m_send_que.push_back(dc);
	}

	int CxTcpClient::SendDataIme(const void* buf, int sz)
	{
		m_tiLastSend = time(NULL);
		return ::send(ConnectSocket, (const char*)buf, (int)sz, 0);
	}

	void CxTcpClient::AddListener(CxTcpClientListener* pListener)
	{
		m_lis.insert(pListener);
	}

	void CxTcpClient::RemoveListener(CxTcpClientListener* pListener)
	{
		m_lis.erase(pListener);
	}

	void CxTcpClient::SetStringLanguageSource(GETSTRINGBYINDEX gstr_func)
	{
		m_gstr_func = gstr_func;
	}

	void CxTcpClient::Clear()
	{
		m_send_bytes=	m_recv_bytes=	m_send_tick=	m_recv_tick=0;

		m_bConnect= m_bReady= m_bSSL=false;
	}

	XSOCKET CxTcpClient::GetSocket()
	{
		return ConnectSocket;
	}

	void CxTcpClient::DispatchEvent(int state_name, int state_val,int ab)
	{
		for (auto it : m_lis)
		{
			(it)->OnTcpClientStateChange(this, state_name, state_val,ab);
		}
	}

	void CxTcpClient::OnConnectAfter(int _erno)
	{
		m_bConnect = (_erno == 0) ? true : false;
		if(m_bConnect){
			fprintf(stderr,"connected %s:%d\n",m_ipSvr.c_str(),m_portSvr);
		}
		DispatchEvent(XTCS_CONNECT, _erno, 1);
	}

	int CxTcpClient::GetLastErrorCode()
	{
#ifdef _MSC_VER
		return WSAGetLastError();
#else
		int error=-1;
		socklen_t len=sizeof(socklen_t);
		getsockopt(ConnectSocket, SOL_SOCKET, SO_ERROR, (void*)&error, (socklen_t*)&len);
		return error;
#endif
	}

	void CxTcpClient::SendBreakHeart()
	{

	}

};




namespace Zx
{
	XSOCKET SocketConnectEx(std::string host, int port)
	{
		XSOCKET sk;
		return sk;
	}

};