
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
	private:
		static GxPlayer* _myself;

	public:
		string m_acct_uuid; //�˺� Ψһ�� ����Ϸ��

//����������
//		uint64_t m_gold;//���
//		uint64_t m_coin;//��Ϸ��

	public:

		/*
		����Լ�
		*/
		static GxPlayer* Instance();


	};


};




#endif 
