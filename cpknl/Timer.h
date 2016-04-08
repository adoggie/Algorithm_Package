// Timer.h: interface for the CTimer class.
//						��ʱ��
//			���ܣ� ����������������ĵ�ʱ�䳤�ȣ���ȷ��0.001��
//                 ������debug��release ״̬�¶��ܹ�����
//          ���ߣ� ������   ���ɹ�˾ͳ���㷨С��
//			ʱ�䣺 2005��11��2��

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_)
#define AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include "../Public/common.h"
#ifdef WINPLAT
#include <process.h>
#endif

#ifdef _UNIX
	#define _timeb	timeb
	#define _ftime	ftime
#endif 
class CTimer  
{
public:
	CTimer();
	virtual ~CTimer();
	//��ʼ��ʱ��strTitleΪ����ı�ǡ�
	void Tic(CTString strTitle = "");
	void Toc(CTString strTitle = "");//����ʱ�䣬Ϊ��ʱ�ļ�ʱ�� ��λΪ��
	void TotalTime();//����ۼ���ʱ��
    void SetPath(CTString strPathName);//���ô洢·��,���Ұ����ļ�����

private:
   bool     m_bFirst;      //�Ƿ��һ�μ�ʱ
   double   m_dBeginTime;  //��ʼʱ��
   CTString  m_strTitle;      //������� 
   struct _timeb timebuffer_begin;//��ʼʱ��
   struct _timeb timebuffer_end;  //��ֹʱ��
   CTString  m_strPath;      //�洢·��,���Ұ����ļ����� ���硰..\\tladvn\\ʱ�����.txt�� 
private:
   double GetSeconds(struct _timeb& str_t);
   double GetElapsedTime();
};

#endif // !defined(AFX_TIMER1_H__ABE4B207_3CD2_44AE_8E80_AC193653833F__INCLUDED_)
