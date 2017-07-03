
#include "MyTcpClient.h"
//#include "CCPlatformMacros.h"
#include "cocos2d.h"

#ifdef  _MSC_VER
//#define _WINDOWS 
//#define Z_SOLO
#endif //  _MSC_VER


#include "zlib/include/zlib.h"

#include <fstream>

#include "game/opcodes.h"
#include "game/xgame.h"

using namespace cocos2d;

typedef unsigned char uint8;
#define X_ATTR_DZIP_MARK	"XDZIP"

#define segment_size 1460//largest tcp data segment




//#include "zlib_wrapper.h"

bool zip_buffer(const char *src_buffer, const unsigned int src_len,
	std::vector<char> &result_buffer)
{
	if ((NULL == src_buffer) || (0 == src_len))
	{
		return false;
	}
	//得到要保存压缩结果的最小buffer大小
	unsigned long min_length = compressBound(src_len);
	result_buffer.resize(min_length);
	if (Z_OK != compress((Bytef*)&result_buffer[0],
		&min_length, (Bytef*)src_buffer, src_len))
	{//压缩失败
		return false;
	}
	result_buffer.resize(min_length);

	return true;
}

unsigned int get_temp_buffer_size(const unsigned int src_length)
{
	const int MB = 1024 * 1024;
	if (src_length < 1 * MB)
	{
		return 1 * MB;
	}
	else if ((src_length >= 1 * MB) && (src_length < 8 * MB))
	{
		return 8 * MB;
	}
	else
	{
		return 16 * MB;
	}
}

bool unzip_buffer(const char *src_buffer, const unsigned int src_len,
	std::vector<char> &result_buffer)
{
	if ((NULL == src_buffer) || (0 == src_len))
	{
		return false;
	}

	

	//提供临时缓存，保存解压的中间结果
	const unsigned int temp_buffer_size = get_temp_buffer_size(src_len);
	std::vector<char> temp_buffer(temp_buffer_size);
	//使用默认值
	z_stream zs;
	zs.zalloc = Z_NULL;
	zs.zfree = Z_NULL;
	zs.next_in = (Bytef*)src_buffer;
	zs.avail_in = src_len;
	zs.next_out = (Bytef*)&temp_buffer[0];
	zs.avail_out = temp_buffer_size;
	zs.opaque = NULL;
	if (Z_OK != inflateInit(&zs))
	{
		return false;
	}

	bool decompress_result = true;
	for (;;)
	{
		int ret_val = inflate(&zs, Z_SYNC_FLUSH);
		if ((Z_OK != ret_val) && (Z_STREAM_END != ret_val))
		{
			//发生错误
			decompress_result = false;
			break;
		}
		//得到缓存里面有效数据长度
		unsigned int valid_data_size = temp_buffer_size - zs.avail_out;
		result_buffer.insert(result_buffer.end(),
			&temp_buffer[0], &temp_buffer[0] + valid_data_size);

		if (Z_OK == ret_val)
		{//待解压数据没有处理完
		 //每次把输出缓冲区重置，免得每次都去计算输出缓冲区应该开始的地址
			zs.next_out = (Bytef*)&temp_buffer[0];
			zs.avail_out = temp_buffer_size;
		}
		else if (Z_STREAM_END == ret_val)
		{//所有数据处理完成
			break;
		}
	}
	inflateEnd(&zs);

	return decompress_result;
}




int XDataCompress(std::vector<uint8>& outStr, const char* inStr, uLong inLength)
{
	uLong comprLen = compressBound(inLength);// X_4M * 2;
	outStr.resize(comprLen);
	int err = compress((Bytef*)&outStr[0], &comprLen, (const Bytef*)inStr, (uLong)inLength);
	//CHECK_ERR(err, "compress");
	assert(err == 0);
	outStr.resize(comprLen);
	outStr.resize(comprLen + sizeof(uLong) + 5);
	memcpy(&outStr[comprLen], &inLength, sizeof(uLong)); //最后面加上长度
														 //最后面加上压缩的标记
	memcpy(&outStr[comprLen + sizeof(uLong)], X_ATTR_DZIP_MARK, 5);
	return err;
}


int XDataUncompress(std::vector<uint8>& outStr, const char* inStr, uLong inLength)
{
	uLong comprLen = 0;// X_4M * 2;
	memcpy(&comprLen, inStr + inLength - sizeof(uLong), sizeof(uLong));
	outStr.resize(comprLen);
	int err = uncompress((Bytef*)&outStr[0], &comprLen, (const Bytef*)inStr, inLength);
	//CHECK_ERR(err, "uncompress");// Z_MEM_ERROR Z_BUF_ERROR
	assert(err == 0);
	outStr.resize(comprLen);
	return err;
}


/* Compress gzip data */
/* data 原数据 ndata 原数据长度 zdata 压缩后数据 nzdata 压缩后长度 */
int gzcompress(Bytef *data, uLong ndata,
	Bytef *zdata, uLong *nzdata)
{
	z_stream c_stream;
	int err = 0;

	if (data && ndata > 0) {
		c_stream.zalloc = NULL;
		c_stream.zfree = NULL;
		c_stream.opaque = NULL;
		//只有设置为MAX_WBITS + 16才能在在压缩文本中带header和trailer
		if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) return -1;
		c_stream.next_in  = data;
		c_stream.avail_in  = ndata;
		c_stream.next_out = zdata;
		c_stream.avail_out  = *nzdata;
		while (c_stream.avail_in != 0 && c_stream.total_out < *nzdata) {
			if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK) return -1;
		}
		if (c_stream.avail_in != 0) return c_stream.avail_in;
		for (;;) {
			if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END) break;
			if (err != Z_OK) return -1;
		}
		if (deflateEnd(&c_stream) != Z_OK) return -1;
		*nzdata = c_stream.total_out;
		return 0;
	}
	return -1;
}

/* Uncompress gzip data */
/* zdata 数据 nzdata 原数据长度 data 解压后数据 ndata 解压后长度 */
int gzdecompress(Byte *zdata, uLong nzdata,
	Byte *data, uLong *ndata)
{
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	static char dummy_head[2] = {
		0x8 + 0x7 * 0x10,
		(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	};
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = data;
	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;
	while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK) {
			if (err == Z_DATA_ERROR) {
				d_stream.next_in = (Bytef*)dummy_head;
				d_stream.avail_in = sizeof(dummy_head);
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					return -1;
				}
			}
			else return -1;
		}
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;
	*ndata = d_stream.total_out;
	return 0;
}


#define CHECK_ERR(err, msg) { \
	if (err != Z_OK) { \
		fprintf(stderr, "%s error: %d\n", msg, err); \
	} \
}

void* my_alloc_func(void* opaque, uInt items, uInt size)
{
	return realloc(opaque, items*size);
}

void  my_free_func(void* opaque, void* address)
{
	//char* p = (char*)opaque;
	//char* p1 = (char*)address;
	//p = p + p1;
	//free(p);
}

/* ===========================================================================
* Test inflate() with large buffers
*/
//void X_large_inflate(Byte * compr, uLong comprLen, Byte * uncompr, uLong* uncomprLen)
int X_large_inflate(Byte *zdata, uLong nzdata,
	Byte *data, uLong *ndata, std::stringstream& sss)
{
	//std::stringstream sss;
	uInt bsz = 16384;
	char buffer[16384+1];
	int err = 0;
	z_stream d_stream = { 0 }; /* decompression stream */
	//static char dummy_head[2] = {
	//	0x8 + 0x7 * 0x10,
	//	(((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
	//};
	d_stream.zalloc = NULL;
	d_stream.zfree = NULL;
	d_stream.opaque = NULL;
	d_stream.next_in = zdata;
	d_stream.avail_in = 0;
	d_stream.next_out = (Byte*)buffer;
	
	uInt ibz = d_stream.total_out;

	//只有设置为MAX_WBITS + 16才能在解压带header和trailer的文本
	if (inflateInit2(&d_stream, MAX_WBITS + 16) != Z_OK) return -1;
	//if(inflateInit2(&d_stream, 47) != Z_OK) return -1;

	while (/*d_stream.total_out < *ndata &&*/ d_stream.total_in < nzdata) {
		ibz = d_stream.total_out;
		d_stream.avail_in = 512;
		d_stream.avail_out = bsz; /* force small buffers */
//		d_stream.next_in = (Bytef*)zdata + bsz;
		d_stream.next_out = (Byte*)buffer;
		if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
		if (err != Z_OK) {
			if (err == Z_DATA_ERROR) 
			{
				//d_stream.next_in = (Bytef*)zdata+ d_stream.total_in;
				//d_stream.avail_in = 1;
				if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) {
					return -1;
				}
			}
			else return -1;
		}
		else {
			ibz = d_stream.total_out - ibz;
			sss.write(buffer, ibz);
		}
	}
	if (inflateEnd(&d_stream) != Z_OK) return -1;

	ibz = d_stream.total_out - ibz;
	sss.write(buffer, ibz);

	*ndata = d_stream.total_out;
	return 0;
}



CCTcpClient* CCTcpClient::_shared = NULL;

void CCTcpClient::Step()
{
	m_tcpClient.Step();

	//


}

CCTcpClient* CCTcpClient::shared()
{
	if (_shared == NULL) {
		_shared = new CCTcpClient();
	}
	return _shared;
}

CCTcpClient::CCTcpClient()
{
	xs::CxTcpClient::Startup();

	
	m_tcpClient.AddListener(this);
}

CCTcpClient::~CCTcpClient()
{
	
}

int CCTcpClient::Connect(std::string host, int port)
{
	return m_tcpClient.Connect(host, port);
}




int CCTcpClient::SendToServer(const char* buf, int sz)
{
	m_tcpClient.SendData(buf, sz);
	return 0;
}

void CCTcpClient::OnTcpClientDataRecv(CxTcpClient* sender, const char* _txt, int sz)
{
	//if (sender == &m_tcpClient)
	//{

	//	{
	//		Node* n = Director::getInstance()->getRunningScene()->getChildByName("main");
	//		if (n) {
	//			n->OnMessageRecv((char*)_txt, sz, 0);
	//			GxMsgQueue::shared()->LoopWork([](const char * buf, size_t sz, const void* who, std::string& name, void * arg)
	//			{
	//				Node* n = (Node*)arg;
	//				n->OnMsg(buf, sz, who, name, arg);
	//			}, n);
	//		}
	//	}
	//}
}


void CCTcpClient::OnTcpClientStateChange(CxTcpClient* sender, int state_name, int state_val, int arg)
{
	if (sender == &m_tcpClient)
	{
		//if(state_name==)
		//		GxMsgQueue::shared()->PushBack(0, 0, 0, "OnTcpClientStateChange");
	}
}



void DIRECTOR_TICK_TCPCLIENT(float,void* arg)
{
	CCTcpClient* tcp = (CCTcpClient*)arg;
	tcp->Step();
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

void XzOnNetMessage(void* wnd, char* buf, size_t sz, void* arg)
{
	Node* n = (Node*)wnd;
	if (n) n->OnMessage(buf, sz, arg);
}

void XzAppMessagePushBack2(void* wnd, const char* buf, size_t sz, void* who, bool zip)
{
	if (zip)
	{
		//char outByte[40960];
		uLong outLen= get_temp_buffer_size(sz);
		char* outByte = new char[outLen];
		
		//91,161

		//std::vector<uint8> res;
		//XDataUncompress(res, buf, sz);
		gzdecompress((Byte*)buf, sz, (Byte*)outByte, &outLen);
		//std::vector<char> res;
		//bool b= unzip_buffer(buf, sz, res);
		//if (b)
		{
			std::ofstream ofs;
			ofs.open("d:\\1.txt", std::ios_base::binary);
			if (ofs.good()) {
				ofs.write((const char*)outByte, outLen);
				ofs.close();
			}
		}
		delete[] outByte;
	}

	//Director::getInstance()->
}


void XzAppMessagePushBack(std::string kname,void* wnd, const char* buf, size_t sz, void* who, bool zip)
{
	if (zip)
	{
		//char outByte[40960];
		uLong outLen =  get_temp_buffer_size(sz);
		//char* outByte = new char[outLen];

		//91,161
		std::stringstream sss;
		//std::vector<uint8> res;
		//XDataUncompress(res, buf, sz);
		X_large_inflate((Byte*)buf, sz, (Byte*)0, &outLen,sss);
		//std::vector<char> res;
		//bool b= unzip_buffer(buf, sz, res);
		if (0)
		{
			std::ofstream ofs;
			ofs.open("d:\\1.txt", std::ios_base::binary);
			if (ofs.good()) {
				//ofs.write((const char*)outByte, outLen);
				ofs << sss.str();
				ofs.close();
			}
		}
		//delete[] outByte;
		std::string str = sss.str();

		//前面加上内容
		if (kname.compare(X_IDS_AUTH_URL_PATH) == 0)
		{
			uint16_t opc = XSMSG_AUTH_URL;
			string s;
			s.append((char*)&opc, 2);

			int posA = str.find(X_MARK_L);
			int posB = str.find(X_MARK_R, posA);

			string site = str.substr(posA + strlen(X_MARK_L), posB - posA - strlen(X_MARK_L));

			s += site;
			Director::getInstance()->MsgPushBack(s.c_str(), s.length(), who);
			return;
		}

		Director::getInstance()->MsgPushBack(str.c_str(), str.length(), who);
	}

	//
}

