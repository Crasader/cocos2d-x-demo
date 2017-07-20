
#include "CxTcpClient.h"
#include "xnet.h"
#include "XStrHelper.h"

#include <chrono>

#define XSTR_NET_BH  "@!ECHO 0123456789"



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
		SetProtocol(1, "\r\n\r\n", 4);
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
		//OnConnectAfter(0);
		DispatchEvent(XTCS_DISCONNECT, 0, 1);
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
					//int _delta = 0;
					//if (m_pto_type == 0) _delta = 4;
					/*
					if (0 == ExecuteCmdline(m_buffer, rc))
					{
						for (auto it : m_lis)
						{
							(it)->OnTcpClientDataRecv(this, m_buffer, rc);
						}
					}
					*/
					Write(m_buffer, rc);

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
//		std::lock_guard<std::mutex> lck(m_mtx_send_que);
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
//		std::lock_guard<std::mutex> lck(m_mtx_send_que);
		
		CxDatChunk* dc = new CxDatChunk();

		if (m_pto_type == 0)
		{
			dc->SetDataV(2,  4, &sz,  sz,buf );
		}
		else {
			dc->SetDataV(2,  sz, buf,  m_pto_emark.length(),m_pto_emark.c_str() );
		}

		//dc->SetData(buf, sz);
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
		SetProtocol(1, "\r\n\r\n", 4);
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
		DispatchEvent(XTCS_CONNECT, 1, _erno);
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
		std::string str;
		str.append(XSTR_NET_BH);

		std::chrono::steady_clock::time_point my_stamp = std::chrono::steady_clock::now();


		str.append((char*)&my_stamp, sizeof(my_stamp));

		static size_t echo_bh_len = str.length();

		SendData(str.c_str(), echo_bh_len);
	}

	void CxTcpClient::Write(const char* buf, size_t sz)
	{
		m_input.write(buf, sz);
		while (1)
		{
			CxDChunk* chunk = NULL;

			if (m_pto_type == 1) {
				chunk = m_input.getChunkEMark2("\r\n\r\n", 4);
			}
			else {
				chunk = m_input.getChunkPreUint32();
			}

			if (chunk == NULL) break;

			m_recv_tick++;

			//		DoCmd(chunk->c_str(), chunk->length()); //

#if(0)
			m_recv_bytes += chunk->length();

			printf("收数:%llu,收包:%llu 本:%d\n",
				GetRecvBytes(),
				GetRecvTick(),
				chunk->length());
#endif
			
			//if (m_delegate) m_delegate->OnRecvPack(this, chunk->c_str(), chunk->length());

			if (0 == ExecuteCmdline(chunk->c_str(), chunk->length()))
			{
				for (auto it : m_lis)
				{
					(it)->OnTcpClientDataRecv(this, chunk->c_str(), chunk->length());
				}
			}
			else {
				m_input.clear();
			}

			delete chunk;
		}




		//printf("收数:%llu,收包:%llu 本:%d\n",
		//	GetRecvBytes(),
		//	GetRecvTick(),
		//	size);

		//处理默认命令
		//if (0 != DoCmd(buf, size)) {
		//	m_input.clear();
		//	return; //如果是命令就直接处理掉 不往下处理
		//}
	}

	int CxTcpClient::ExecuteCmdline(const char* _buf, size_t _size)
	{
		int r = 0;

		if (_size < 3) return 0;
		if (_buf[0] != '@' || _buf[1] != '!') return 0;

		//if (m_delegate) r = m_delegate->OnCommand(this, _buf, _size);
		//if (r != 0) return r;

		//执行默认的
		//HELO
		if (strstr(_buf, "@!HELO") == _buf)
		{
			//TODO 更多的命令处理

			//	snprintf(buf, 1024, "@!HELO\nk:xxxx\nPTO_TYPE:1\nPTO_EMARK:%0d%0a%0d%0a\nPCODE:12\n");
			//_sender->Reset();
			//查看通信协议方式
			std::string value = XStrHelper::GetUrlValueString(_buf, _size, "PTO_TYPE");
			if (!value.empty()) {
				int _ptype = atoi(value.c_str());
				std::string _em = XStrHelper::GetUrlValueString(_buf, _size, "PTO_EMARK");
				//对_em进行转码
				if (!_em.empty()) _em = XStrHelper::DecodeURL(_em);
				//SetPtoType(_ptype, _em.c_str(), (int)_em.length()); //避免回环
				m_pto_type = _ptype;

				//此设定发起人
				std::string mt = XStrHelper::GetUrlValueString(_buf, _size, "M_TYPE");
				if (0 == mt.compare("S")) {
					//服务器端发起的 就此回复给服务器
					//Send(_buf, _size);
					xnet_send(ConnectSocket, _buf, _size);
				}

			}

			m_input.clear();

			//准备就绪允许上层进行数据发送
			m_bReady = true;

			//if (m_delegate) m_delegate->OnReady(this);
			this->DispatchEvent(XTCS_READY, m_bReady, 1);

			SendBreakHeart();

			return 1;
		}


		//如果是回声
		if (strstr(_buf, "@!ECHO 0123456789") == _buf)
		{
			std::chrono::steady_clock::time_point start;
			memcpy((char*)&start, _buf + strlen(XSTR_NET_BH), sizeof(start));
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			int64_t tick= std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

			printf("本次回声耗时 [%zu]us 微秒 ", tick);
			return 1;
		}



		return 0;
	}

	void CxTcpClient::SetProtocol(int _type, const char* _emark, size_t _size)
	{
		//发送这个协议后，在后面还没收到反馈的时候，收到的消息协议都丢弃掉
		if (m_bConnect)
		{
			m_bReady = false;
			char buf[256];
			//snprintf(buf, 1024, "@!HELO\nk:xxxx\nPTO_TYPE:1\nPTO_EMARK:%0d%0a%0d%0a\nPCODE:12\n");
			snprintf(buf, 256, "@!HELO k:xxxx PTO_TYPE:%d PTO_EMARK:%%0d%%0a%%0d%%0a M_TYPE:C PCODE:12\r\n\r\n",
				_type);
			//SendData(buf, (int)strlen(buf));
			xnet_send(ConnectSocket, buf, (int)strlen(buf));
			//return;
		}

		//if (m_pto_type == _type) return;

		m_pto_type = _type;
		if (_emark == NULL) _emark = "\r\n\r\n";
		if (_size < 1) _size = (int)strlen(_emark);
		m_pto_emark.append(_emark, _size);
		m_input.clear();
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