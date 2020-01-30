//----------------------------------------
// (C) Лапшин E.O., 1997
//---------------------------------------

#include <dos.h>

#include "timer.h"
#include "sprite.h"
#include "screen.h"
#include "debug.h"

//создаем предопределенный объект
TIMER Timer;

//************************************************************
//'timer.cpp' функции-члены класса TIMER для работы с таймером
//************************************************************

#define TIMER_INT	0x1c

unsigned long	int08h_old;

//инициализируем статические переменные
	int	TIMER::CountObjects=0;
	const float TIMER::MAX_FREQ=48000u;
	const float TIMER::MIN_FREQ=18.20648;


//обработчик прерываний таймера
void interrupt int08h_new1()
{
        extern TIMER Timer;
	Timer.Counter++;
        Timer.TotalCounter++;
}

// КОНСТРУКТОР таймера
// int freq - частота таймера (MIN_FREQ - MAX_FREQ)
TIMER::TIMER(float freq)
{
	CountObjects++;		//увеличиваем число созданных объектов

	#if DEBUGMESSAGES
		Debug("Run TIMER constructor");
	#endif

	//запомним частоту в переменной члена-класса
	//запоминаем старый вектор прерывания 8
	int08h_old =(long) _dos_getvect(TIMER_INT);

	//устанавливаем вектор прерывания таймера
	asm {
		xor ax,ax
		mov es,ax
		mov ax,offset int08h_new1
		mov es:[0x1c*4],ax
		mov ax,seg int08h_new1
		mov es:[0x1c*4+2],ax
	}

	//изменяем частоту таймера
	Restart(freq);
}

//перезапускаем таймер с новой частотой
//минимальная частота должна быть 1193180/65536=18.20648 герц
//максимальная частота может быть 1193180/1=1193180 герц
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

	//изменяем частоту таймера
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

        //изменяем переменную-члена класса
        {
		Freq=freq;
        }

        //обнуляем счетчики
        {
		Counter=0;
        	TotalCounter=0;
        }
}

void GetRealTimeClock(unsigned char &hour, unsigned char &min, unsigned char &sec)
{
	asm mov ah,0x02	//получаем время из часов реального время
	asm int 0x1a
	hour=_CH;
	min=_CL;
	sec=_DH;
	//переводим из двоично-десятичной в десятичный
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

//=====================деструктор=====================
TIMER::~TIMER()
{
    if (CountObjects!=0)	//выполняем деструктивные действия
				//только если объект был создан
    {
	CountObjects--;
	#if DEBUGMESSAGES
		Debug("TIMER destructor started");
	#endif

	//восстанавливаем старый обработчик
	//прерывания int 0x08 _dos_setvect(TIME_KEEPER_INT, int08h_new);
	asm {
		xor ax,ax
		mov es,ax
		mov ax,word ptr int08h_old
		cli	//запрещаем прерывания на время изменени
			//таблицы векторов прерывания
		mov es:[0x1c*4],ax
		mov ax,word ptr int08h_old+2
		mov es:[0x1c*4+2],ax
		sti
	}

	//восстанавливаем частоту таймера
	unsigned deliver=0; // фактически 65536
	asm {
		mov al,0x3c
		out 0x43,al
		mov al,byte ptr deliver
		out 0x40,al
		mov al,byte ptr deliver+1
		out 0x40,al
	}


	unsigned char hour,min,sec;
	asm mov ah,0x02	//получаем время из часов реального время
	asm int 0x1a
	hour=_CH;
	min=_CL;
	sec=_DH;
	//переводим из двоично-десятичной в десятичный
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

     //если объект еще не был создан
     else
     {
		Debug( "TIMER-destructor -> object not created!");
     }
}
