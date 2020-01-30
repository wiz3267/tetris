#include "mario.h"
#include "timer.h"

#include <stdlib.h>

extern SPRITE BigFont;
extern SPRITE Sprite;
extern LEVEL Level;

#include "sprite.h"
#include "level.h"
#include "keyboard.h"

#include "idsprite.h"

void Update()
{
	scr.update();
}

void Synh()
{
	while (!Timer.Counter) {}
	Timer.Counter=0;
}

int SUPERMARIO_menu_show_zast=1;
extern SUPERMARIO Supermario;

void StartPicture()
{
   if (SUPERMARIO_menu_show_zast)
   {
	Supermario.music(3);

        //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//$$$$$$$$$$$$$$$ 1-ая часть - вывод российского флага $$$$$$$$
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

	float angle=0,step=0.05, ampl=3;

	int DecrementPal=0;
	int DELAY=110;
	int NEXT=200;

	float ddy=40-12;	//для надписи

	int FlagLen=150;
	int FlagHei=150/2;


	Timer.Restart(50);
	Timer.Counter=0;

	float dy=70;     //для флага

/*	//пока не нажата любая клавиша
	while ( DecrementPal<DELAY )
	{

	scr.clear();


	//рисуем флаг
	{
		int dx= (320-FlagLen) / 2;
		float a=angle;
		for(int x=0; x<FlagLen; x++, a+=step )
		{
			int ry=dy+sin(a)*ampl;
			int rx=dx+ x;
			int len=FlagHei/3;
			scr.line_down(rx, ry, len, 255);
			scr.line_down(rx, ry+len, len,  207);
			scr.line_down(rx, ry+2*len, len, 159);
		}
	}

	angle+=step*3;

	if (Timer.GetTotalCounter()>NEXT)
	{
		dy-=1;
		ddy+=1;
	}

	if (Timer.GetTotalCounter()>NEXT && !DecrementPal)
	{
		DecrementPal=1;
	}

	if (dy<-100) break;

	Update();

	if ( Keyb.AnyKey() && !DecrementPal)
	{
		DecrementPal=1;
	}

	if (DecrementPal)
	{
		scr.DecrementPalette();
		DecrementPal++;
	}

	Synh();

      }*/

/*
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//$$$$$$$$$$$$$$$ 2-ая часть - вывод надписи PRESENTS $$$$$$$$$
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	Timer.Restart(50);
	scr.graphmode();

	DecrementPal=0;

	Timer.Counter=0;

	//пока не нажата любая клавиша
	while ( DecrementPal<DELAY )
	{

	scr.clear();

	string s1(&BigFont,"PRESENTS");

	s1.Show((320-8*24)/2,80);

	Update();

	if ( Keyb.AnyKey() && !DecrementPal)
	{
		DecrementPal=1;
	}

	if ( Timer.GetTotalCounter()>NEXT && !DecrementPal)
	{
		DecrementPal=1;
	}

	if (DecrementPal)
	{
		scr.DecrementPalette();
		DecrementPal++;
	}

	Synh();
      }

	Timer.Counter=0;
      */

	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//$$$$$$$$$$$$$$$ 2.1-ая часть - вывод надписи SOFMARIO $$$$$$$$$
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	Timer.Restart(50);
    {

	scr.graphmode();

	DecrementPal=0;


	string s1(&BigFont,"SUPER");
	string s2(&BigFont,"");
	string s3(&BigFont,"JACKS");



	//пока не нажата любая клавиша
	while ( DecrementPal<DELAY )
	{

	scr.clear();

	{
		int d=-30;
		s1.Show(80+d);
		s2.Show(110+d);
		s3.Show(140+d);
	}

	Update();

	if ( Keyb.AnyKey() && !DecrementPal)
	{
		DecrementPal=1;
	}

	if ( Timer.GetTotalCounter()>NEXT && !DecrementPal)
	{
		DecrementPal=1;
	}

	if (DecrementPal)
	{
		scr.DecrementPalette();
		DecrementPal++;
	}
	Synh();
      }

    }
 }

	SUPERMARIO_menu_show_zast=0;
}