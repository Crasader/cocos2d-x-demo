
#ifndef XPlayer_h__
#define XPlayer_h__

#include "XActor.h"

namespace xg
{
	/**
	��ͬ��ҿ��Ƶ���Ա
	*/
	class GxPlayer : public CxActor
	{


	public:
		string m_acct_uuid; //�˺� Ψһ�� ����Ϸ��

		uint64_t char_guid;

//����������
//		uint64_t m_gold;//���
//		uint64_t m_coin;//��Ϸ��

		string m_body_part;
	public:




	};


};




#endif 
