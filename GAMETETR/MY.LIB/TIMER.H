#ifndef __TIMER_H
#define __TIMER_H

//******************************************************************
//******* TIMER.H заголовочный файл для работы с таймером **********
//******************************************************************

class TIMER
{
	static int CountObjects;	//число созданных объектов типа TIMER
	unsigned long TotalCounter;	//счетчик числа прерывания с начала инициализации таймера

//******************** ДАННЫЕ-ЧЛЕНЫ ***************
public:
	int Counter;		//счетчик числа прерываний
	int Freq;		//частота таймера,Герц
	static const float MIN_FREQ;	//минимальная частота
	static const float MAX_FREQ;	//максимальная частота


//********************** ФУНКЦИИ-ЧЛЕНЫ **************************
public:
	void interrupt int08h_new(void);//функция-обработчик прерываний
//	int  GetCounter();		//получение значения счетчика
	friend void interrupt int08h_new1(void);
	unsigned long GetTotalCounter() { return TotalCounter; };
	void Restart(float Freq);	//рестарт таймера с новой частотой
//	void SetCounter(int count);	//установка значения счетчика
	TIMER(float freq=MIN_FREQ);	//инициализирует вектор прерывания 0x08
	~TIMER(void);			//восстанавливает частоту
};

extern TIMER Timer;

#endif