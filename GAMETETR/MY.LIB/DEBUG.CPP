////////////////////////////////////////
// Copyright (C) Лапшин Евгений, 1997 //
////////////////////////////////////////

#include <conio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>

#include "screen.h"
#include "debug.h"
#include "keyboard.h"


#define DEBUGMESSAGES 1

int MsgTextMinMaxValue(char far *msg, int min_value, int max_value, int current_value, int y_coor)
{
//	int val=1;

	char color1=20;
	char color2=9;

	float	fval = ((float)(current_value - min_value)) /
	(max_value - min_value);

	float	fval_delta = 0.05;
	float	fval_min = 0.0001;
	float	fval_max = 1 - 0.0001;
	int		scr_x1 = 30;
	int		scr_x2 = 200;
	int		scr_y1 = y_coor;

	do {
	//scr.clear();

	scr.x=30;
	scr.y=scr_y1-8;

	scr.color=15;
	scr.bkcolor=0;
	scr<<msg;
	scr.bkcolor=16;
	scr.color=9;
	scr<<" ";
	int v=min_value + (max_value - min_value) * fval;
	scr.putNumb(v);


//	scr.border(0+val,0+val,60,60, val);
//	scr.line_down(0,0, 30, 4);
//	scr.line_right(30,40, 90, 4);

	int hei=10;
	for( int i = 0; i < hei; i++ )
	{
		scr.line_right(scr_x1, scr_y1 + i, (scr_x2 - scr_x1), color1);
		scr.line_right(scr_x1, scr_y1 + i, (scr_x2 - scr_x1) * fval, color2);
	}
	scr.border (scr_x1, scr_y1, scr_x2 - scr_x1, hei, 14);

	if (Keyb[KEY_LEFT])
	{
		fval -= fval_delta;
		if (fval < fval_min)
			fval = fval_min;
  //	if (val) val--;
		Keyb.Set(KEY_LEFT,0);
	}

	if (Keyb[KEY_RIGHT])
	{
		fval += fval_delta;
		if (fval > fval_max)
			fval = fval_max;
//		if (val<100) val++;
		Keyb.Set(KEY_RIGHT,0);
	}

	if (Keyb[KEY_ENTER])
	{
	  return ((int)(min_value + (max_value - min_value) * fval));
	}

	scr.update();

	} while(1);


//	return val;
}

int MsgText(char far *msg, char far * text1, char far * text2, int bool_init, int y_coor)
{
	int x,y,len,hei;

	int oldbkcolor=scr.bkcolor;
	int oldcolor=scr.color;

	SCREEN &sc=scr;

	if (bool_init>1) bool_init=1;
	if (bool_init<0) bool_init=0;

	int color=1;

	int bool=bool_init;
rest:


	len=strlen(msg)+1+1+strlen(text1)+1 +1 +1+strlen(text2)+1;

	x=(320-len*scr.StepX)/2;
	hei=8*3;

	if (y_coor==0)	y=11;
	else y=y_coor;



//	x-=1;
	sc.setcolor(15);
	sc.bkcolor=color;
	sc.rectangle(x, y*8, len*scr.StepX, hei, color );

	sc.locateXY(x,y*8+ hei/2-4);
	sc.putText(msg);
	sc.bkcolor=color;
	sc.putText(" ");

	if (bool==0)
	{
		sc.bkcolor=15;
		sc.color=2;
	}
	else
	{
		sc.bkcolor=1;
		sc.color=15;
	}
	sc.putText(" ");
	sc.putText(text1);
	sc.putText(" ");

	sc.bkcolor=color;
	sc.putText(" ");

	if (bool==1)
	{
		sc.bkcolor=15;
		sc.color=2;
	}
	else
	{
		sc.bkcolor=1;
		sc.color=15;
	}
	sc.putText(" ");
	sc.putText(text2);
	sc.putText(" ");

	sc.update();

	char key=Keyb.ReadScan();

	if (key == KEY_LEFT)
	{
		bool--;
		if (bool==-1) bool=1;
	}
	if (key == KEY_RIGHT)
	{
		bool++;
		if (bool==2) bool=0;
	}

	if (key == KEY_ENTER)
	{
		scr.color=oldcolor;
		scr.bkcolor=oldbkcolor;

		return bool;
	}

	goto rest;


}

//выводит сообщение
void Msg(char far *text,int color)
{
	int x,y,len,hei;
	int oldbkcolor,oldcolor;

	//oldbkcolor=scr.bkcolor;
	//oldcolor=scr.color;

	SCREEN &sc=scr;

	len=strlen(text);

	//если сообщение полностью помещается на экране
	if (len<=60)
	{
		x=(320-len*scr.StepX)/2;

		hei=8*3;

		y=11*8;
	}

	sc.rectangle(x, y, len*scr.StepX, hei, color );

	sc.locateXY(x,y);
	//sc.setcolor(15);
	sc.bkcolor=color;

	sc.locateXY(x,y+(hei-8)/2);

	sc.putText(text);

	int i=strlen(text)-1;

	while (text[i]==' ' && i)
	{
		sc.x-=scr.StepX;
		i--;
	}
	sc.x+=scr.StepX;
	sc.update();

	//scr.color=oldcolor;
	//scr.bkcolor=oldbkcolor;
	int a=3;
	a++;

}

//== 'fatal' - выдача сообщения об ошибке и завершение работы программы ===
void fatal(char far * str)
{
	SCREEN sc;
	Debug("******** Fatal Error ******");
	Debug(str);
//	asm int 3;

	//asm mov ax,0x3
	//asm int 0x10
	//cout<<str<<"\n"<<(char) 7;
	//cout<<"*** PLEASE CALL TO (831-30) 5-29-87 Ewgeny, and tell my this message ***\n";

	scr.textmode();
		exit(-1);
//	asm int 3

}

//================= 'Debug' - выдача сообщение об ошибке =================
void Debug(char far * text)
{
	Msg(text);
	extern KEYBOARD Keyb;
	if (Keyb.ReadScan() == KEY_Q) exit(1);
	asm int 3;
}

//====================== end of 'fatal' ===========================

