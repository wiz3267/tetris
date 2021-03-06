#ifndef __TIMER_H
#define __TIMER_H

//******************************************************************
//******* TIMER.H ���������� 䠩� ��� ࠡ��� � ⠩��஬ **********
//******************************************************************

class TIMER
{
	static int CountObjects;	//�᫮ ᮧ������ ��ꥪ⮢ ⨯� TIMER
	unsigned long TotalCounter;	//���稪 �᫠ ���뢠��� � ��砫� ���樠����樨 ⠩���

//******************** ������-����� ***************
public:
	int Counter;		//���稪 �᫠ ���뢠���
	int Freq;		//���� ⠩���,����
	static const float MIN_FREQ;	//�������쭠� ����
	static const float MAX_FREQ;	//���ᨬ��쭠� ����


//********************** �������-����� **************************
public:
	void interrupt int08h_new(void);//�㭪��-��ࠡ��稪 ���뢠���
//	int  GetCounter();		//����祭�� ���祭�� ���稪�
	friend void interrupt int08h_new1(void);
	unsigned long GetTotalCounter() { return TotalCounter; };
	void Restart(float Freq);	//����� ⠩��� � ����� ���⮩
//	void SetCounter(int count);	//��⠭���� ���祭�� ���稪�
	TIMER(float freq=MIN_FREQ);	//���樠������� ����� ���뢠��� 0x08
	~TIMER(void);			//����⠭�������� �����
};

extern TIMER Timer;

#endif