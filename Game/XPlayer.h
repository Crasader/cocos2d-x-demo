
#ifndef XPlayer_h__
#define XPlayer_h__

#include "XActor.h"

namespace xg
{
	/**
	��ͬ��ҿ��Ƶ���Ա
	*/
	class CxPlayer : public CxActor
	{
	private:
		static CxPlayer* _myself;

	public:
		string m_acct_uuid; //�˺� Ψһ�� ����Ϸ��
		string m_session;  //�Ự����

		string m_password; //����

//����������
//		uint64_t m_gold;//���
//		uint64_t m_coin;//��Ϸ��

	public:

		/*
		����Լ�
		*/
		static CxPlayer* Instance();


	};


};




#endif 
