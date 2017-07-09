
#ifndef XTcpClient_h__
#define XTcpClient_h__

#include <string>
#include <stdint.h>
#include <set>
#include <deque>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <time.h>
#include <stdarg.h>
#include <vector>

#ifdef _MSC_VER

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

typedef SOCKET XSOCKET;

#else

typedef int XSOCKET;
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif

#define X_INVALID_SOCKET  (SOCKET)(~0)
#define X_SOCKET_ERROR            (-1)

namespace xs
{

#define X_MALLOC(a,b,c) do{ if(a){ if(b>c) a=(char*)realloc(a,(b>c?b:c)+1);   }else{ a=(char*) malloc(b+1); }   }while(a==NULL); a[b]=0

	enum TCPCLIENT_STATE
	{
		XTCS_UNKNOW,
		XTCS_CONNECT,		//连接
		XTCS_DISCONNECT,	//断开
		XTCS_READY,			//准备就绪

	};

	class CxDatChunk
	{
		char* d;
		int   s;

	public:
		CxDatChunk()
		{
			d = NULL;
			s = 0;
		}

		virtual ~CxDatChunk()
		{
			if (d) free(d);
		}

		char* c_str() { return d; };
		int size() { return s; }

		void SetData(const void* buf, int sz)
		{
			if (sz < 1 || buf == NULL) return;
			X_MALLOC(d, sz + 1, s);
			s = sz;
			memcpy(d, buf, sz);
		}

		void SetDataV(int n, ...)
		{
			int k;
			int a, b;
			s = 0;
			std::vector<int> szv;
			std::vector<const char*> bufv;

			va_list ap;
			va_start(ap, n);

			for (int i = 0; i < n; i++)
			{
				int szi = va_arg(ap, int);
				const char* bfi = va_arg(ap, char*);
				s += szi;
				szv.push_back(szi);
				bufv.push_back(bfi);
			}


			X_MALLOC(d, s,0);
			a = 0; k = 0;
			for (auto it : bufv)
			{
				b = szv[k];
				memcpy(d + a, it, b);
				a = b;
				k++;
			}
			va_end(ap);
		}

	};


	class CxTcpClient;

	class CxTcpClientListener
	{
	public:
		virtual ~CxTcpClientListener(){};
		virtual void OnTcpClientStateChange(CxTcpClient* sender,int state_name,int state_val,int arg) {};
		virtual void OnTcpClientError(CxTcpClient* sender, int _erno) {};
		virtual void OnTcpClientDisplayStringChange(CxTcpClient* sender,const char* _txt,int sz) {};
		virtual void OnTcpClientDataRecv(CxTcpClient* sender, const char* _txt, int sz) {};
		virtual void OnTcpClientNPakRecv(CxTcpClient* sender, const char* _txt, int sz) {};
	};

	//class CxStringLanguageSource
	//{
	//public:
	//	virtual std::string GetStringByIndex(int _ids,int _type=0) = 0;
	//};
	typedef std::string(*GETSTRINGBYINDEX)(int _ids, int _type);

	class CxTcpClient
	{
	public:
		static int Startup();
		static void Shutdown();

		static void SetSocketNoBlock(XSOCKET fd,bool nonblock);

		static CxTcpClient* shared();

		static void FreeShared();

	public:

		CxTcpClient();
		virtual ~CxTcpClient();

		int Connect(std::string ipAddr,int port,int tio=5);
		int Disconnect();

		void RecvIm(char* buffer, int sz);

		void Step();
		
		void SendData(const void* buf, int sz);
		int SendDataIme(const void* buf, int sz);

		void AddListener(CxTcpClientListener* pListener);
		void RemoveListener(CxTcpClientListener* pListener);

		void SetStringLanguageSource(GETSTRINGBYINDEX gstr_func);

		void Clear();

		XSOCKET GetSocket();

	protected:
		/**
		ab 0前 1后
		*/
		void DispatchEvent(int state_name, int state_val, int ab=0);

		void OnConnectAfter(int _erno);

		int GetLastErrorCode();

		bool ExistWaitSend();

		//发送心跳包给服务器 ECHO 返回时间来判断网络情况
		virtual void SendBreakHeart();

	private:
		GETSTRINGBYINDEX m_gstr_func;

	protected:
		uint64_t
			m_send_bytes,	//发送字节数
			m_recv_bytes,	//接收字节数
			m_send_tick,	//发送包次数
			m_recv_tick;	//接收包次数

		


		bool m_bConnect;
		bool m_bReady;
		bool m_bSSL;
		int m_nBreakHeart;	//间隔多少秒向服务器发送一次心跳包

		time_t m_tiLastSend;//最后一次向服务器发送数据时间


		std::set<CxTcpClientListener*> m_lis;

		XSOCKET ConnectSocket;

		fd_set readfds, writefds, exceptfds;

		char m_buffer[8192];

		std::deque<CxDatChunk*> m_send_que;
		std::mutex m_mtx_send_que;

		std::string m_ipSvr;
		int m_portSvr;

	public:
		void* userData;

	};


};


namespace Zx
{
	XSOCKET SocketConnectEx(std::string host,int port);

};




#endif // XTcpClient_h__
