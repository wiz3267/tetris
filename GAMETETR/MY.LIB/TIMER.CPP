//----------------------------------------
// (C) ���設 E.O., 1997
//---------------------------------------

#include <dos.h>

#include "timer.h"
#include "sprite.h"
#include "screen.h"
#include "debug.h"

//ᮧ���� �।��।������ ��ꥪ�
TIMER Timer;

//************************************************************
//'timer.cpp' �㭪樨-童�� ����� TIMER ��� ࠡ��� � ⠩��஬
//************************************************************

#define TIMER_INT	0x1c

unsigned long	int08h_old;

//���樠�����㥬 ����᪨� ��६����
	int	TIMER::CountObjects=0;
	const float TIMER::MAX_FREQ=48000u;
	const float TIMER::MIN_FREQ=18.20648;


//��ࠡ��稪 ���뢠��� ⠩���
void interrupt int08h_new1()
{
        extern TIMER Timer;
	Timer.Counter++;
        Timer.TotalCounter++;
}

// ����������� ⠩���
// int freq - ���� ⠩��� (MIN_FREQ - MAX_FREQ)
TIMER::TIMER(float freq)
{
	CountObjects++;		//㢥��稢��� �᫮ ᮧ������ ��ꥪ⮢

	#if DEBUGMESSAGES
		Debug("Run TIMER constructor");
	#endif

	//�������� ����� � ��६����� 童��-�����
	//���������� ���� ����� ���뢠��� 8
	int08h_old =(long) _dos_getvect(TIMER_INT);

	//��⠭�������� ����� ���뢠��� ⠩���
	asm {
		xor ax,ax
		mov es,ax
		mov ax,offset int08h_new1
		mov es:[0x1c*4],ax
		mov ax,seg int08h_new1
		mov es:[0x1c*4+2],ax
	}

	//�����塞 ����� ⠩���
	Restart(freq);
}

//��१���᪠�� ⠩��� � ����� ���⮩
//�������쭠� ���� ������ ���� 1193180/65536=18.20648 ����
//���ᨬ��쭠� ���� ����� ���� 1193180/1=1193180 ����
void TIMER::Restart(float freq)
{
        if (freq<MIN_FREQ)
        {
        	Debug("TIMER::Restart - freq to smal");
                return;
        }

        if (freq>MAX_FREQ)
        {
        	Debug("TIMER::Restart - freq to big");
                return;
        }

	//�����塞 ����� ⠩���
        {
		unsigned deliver=1193180L/freq;
		asm {
			mov al,0x3c
			out 0x43,al
			mov al,byte ptr deliver
			out 0x40,al
			mov al,byte ptr deliver+1
			out 0x40,al
		}
        }

        //�����塞 ��६�����-童�� �����
        {
		Freq=freq;
        }

        //����塞 ���稪�
        {
		Counter=0;
        	TotalCounter=0;
        }
}

void GetRealTimeClock(unsigned char &hour, unsigned char &min, unsigned char &sec)
{
	asm mov ah,0x02	//����砥� �६� �� �ᮢ ॠ�쭮�� �६�
	asm int 0x1a
	hour=_CH;
	min=_CL;
	sec=_DH;
	//��ॢ���� �� ����筮-�����筮� � �������
	hour=(hour&0x0f) + ((hour&0xf0)>>4)*10;
	min=(min&0x0f) + ((min&0xf0)>>4)*10;
	sec=(sec&0x0f) + ((sec&0xf0)>>4)*10;

	/*float cnt;
	cnt= hour*65543.3349606;
	cnt += min*1092.38891601;
	cnt += sec*18.2064819335;
	if (cnt>1573040l) cnt-=1573040l;
	long t=cnt;
	asm mov cx,word ptr t+2
	asm mov dx,word ptr t
	asm mov ax,0x40
	asm mov es,ax
	asm mov es:[0x6c],dx
	asm mov es:[0x6c+2],cx
    */
}

//=====================��������=====================
TIMER::~TIMER()
{
    if (CountObjects!=0)	//�믮��塞 ������⨢�� ����⢨�
				//⮫쪮 �᫨ ��ꥪ� �� ᮧ���
    {
	CountObjects--;
	#if DEBUGMESSAGES
		Debug("TIMER destructor started");
	#endif

	//����⠭�������� ���� ��ࠡ��稪
	//���뢠��� int 0x08 _dos_setvect(TIME_KEEPER_INT, int08h_new);
	asm {
		xor ax,ax
		mov es,ax
		mov ax,word ptr int08h_old
		cli	//����頥� ���뢠��� �� �६� ��������
			//⠡���� ����஢ ���뢠���
		mov es:[0x1c*4],ax
		mov ax,word ptr int08h_old+2
		mov es:[0x1c*4+2],ax
		sti
	}

	//����⠭�������� ����� ⠩���
	unsigned deliver=0; // 䠪��᪨ 65536
	asm {
		mov al,0x3c
		out 0x43,al
		mov al,byte ptr deliver
		out 0x40,al
		mov al,byte ptr deliver+1
		out 0x40,al
	}


	unsigned char hour,min,sec;
	asm mov ah,0x02	//����砥� �६� �� �ᮢ ॠ�쭮�� �६�
	asm int 0x1a
	hour=_CH;
	min=_CL;
	sec=_DH;
	//��ॢ���� �� ����筮-�����筮� � �������
	hour=(hour&0x0f) + ((hour&0xf0)>>4)*10;
	min=(min&0x0f) + ((min&0xf0)>>4)*10;
	sec=(sec&0x0f) + ((sec&0xf0)>>4)*10;
	float cnt;
	cnt= hour*65543.3349606;
	cnt += min*1092.38891601;
	cnt += sec*18.2064819335;
	if (cnt>1573040l) cnt-=1573040l;
	long t=cnt;
	asm mov cx,word ptr t+2
	asm mov dx,word ptr t
	asm mov ax,0x40
	asm mov es,ax
	asm mov es:[0x6c],dx
	asm mov es:[0x6c+2],cx
     }

     //�᫨ ��ꥪ� �� �� �� ᮧ���
     else
     {
		Debug( "TIMER-destructor -> object not created!");
     }
}
