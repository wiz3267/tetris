#include "debug.h"
#include "screen.h"
#include "timer.h"
#include "keyboard.h"
#include "sound.h"

#include <stdlib.h>
#include "midpak2.h"

void music(int n, int seq);


void DrawBMP(int, int);
char * LoadBMP320x200x256(char *fname);

extern unsigned long wait_flag1;
extern unsigned long wait_flag2;
extern int StepRunLX;

extern int BigSymScale;
void DrawRunLine(int);

	int need_wait=1;
	int Kadr=0;
	int Kadr_Sec=0;
	int show_debug=0;
	int music_loop=1;


int TetrisStart()
{
	char r[256], g[256], b[256];

	StepRunLX=3;

	scr.clear();
	scr.StepX=6;

	for(int i=0; i<256; i++)
	{
		scr.GetColorRegister(i, r[i],g[i],b[i]);
	}

	MidiStop();
	music(0,0);
	//����᪠�� ⠩��� �� �����
	Timer.Restart(70);
	char name[]="bmp\\n1.bmp";
	if (rand()%2) name[5]++;

	if (LoadBMP320x200x256(name))
	{
rest:
		for(int i=0; i<200; i++)
		{
			scr.clear();
			DrawBMP(200-i-1,i);
			scr.wait();
			scr.update();
		}
	}

	for(i=0; i<50*1; i++)
	{
		scr.wait();
		if (Keyb[KEY_ESC])
		{
			Keyb.Set(KEY_ESC,0);
			break;
		}
	}

	unsigned int w_f1,w_f2;


	do {

	if (Keyb[KEY_F1])
	{
		scr.clear();

		scr.rectangle(0,0,320,200,9);
		scr.color=15;
        scr.StepX=6;
		char *hlp="<F1> ������\n"
		"<Q><W> ��������� ᪮���\n"
		"<E> ����� ���⭮�� 室� ���\n"
		"<R> ��१����\n"
		"<Y> �⫠���筠� ���ଠ��\n"
		"<U> ��몠 ���� ࠧ/�����\n"
		"<PAGEUP><PAGEDOWN> ��������� ࠧ��� ⥪��\n"
		"<PAD0> ��砫� 䠩�� \n"
		"<PAD1> ��砩��� ���� \n"
		"<PAD6> ᫥����� ����� \n"
		"<CTRL><F> ���� (shift - ���᪨�)\n"
		"<CTRL><L> ���� �����\n"
		"<ESC> ���室 � ����\n"
		"<F12> ��室 � ��\n"
		;
		scr.x=0;
		scr.y=0;
		scr<<hlp;
		scr.update();
		Keyb.ReadScan();
	}

	Kadr++;

	scr.bkcolor=0;

	DrawBMP(0,200);

	if (show_debug)
	{
	scr.x=0;
	scr.y=0;
	scr<<w_f1<<"\n"<<w_f2<<"\n"<<"SRLX="<<StepRunLX<<"\n";
	scr<<"T.C="<<Timer.Counter<<"\n"<<"T.F="<<Timer.Freq<<"\n";
	scr<<"Kadr="<<Kadr<<"\n";
	scr<<"KadrS="<<Kadr_Sec<<"\n";
	scr<<"need_wait="<<need_wait<<"\n";
	scr<<"music_loop="<<music_loop;
	}

	if (Timer.Counter >= Timer.Freq)
	{
		Kadr_Sec=Kadr;
		Kadr=0;
		Timer.Counter=0;
		scr.x=30;
		scr.y=100;
		scr.color=1;
		//scr.putBigText("��ᯮ��, ������!",1);
	}

//	if (Keyb[KEY_Y]) show_debug^=1, Keyb.Set(KEY_Y,0);
//	if (Keyb[KEY_Q]) StepRunLX--, Keyb.Set(KEY_Q,0);
//	if (Keyb[KEY_W]) StepRunLX++, Keyb.Set(KEY_W,0);
//	if (Keyb[KEY_E]) need_wait^=1, Keyb.Set(KEY_E,0);
//	if (Keyb[KEY_U]) music_loop^=1, Keyb.Set(KEY_U,0);


	//if (StepRunLX<0) StepRunLX=0;
	if (StepRunLX>20) StepRunLX=20;

	scr.SetColorRegister(15, 255,255,255);
	scr.color=15;
	DrawRunLine(200-BigSymScale*8);

	if (need_wait)	scr.wait();

	w_f1=wait_flag1;
	w_f2=wait_flag2;

	scr.update();

	if ( (SequenceStatus() != SEQ_PLAYING) && music_loop)
	{
		music(0,0);
	}
	if (Keyb[KEY_R]) goto rest;
	if (Keyb[KEY_ESC]) break;
	if (Keyb[KEY_F12]) return 0;
	} while (1);

	for(i=0; i<20; i++)
	{
		scr.DecrementPalette();
		scr.DecrementPalette();
		scr.DecrementPalette();
		scr.wait();
	}
	scr.clear();
	scr.update();

	for(i=0; i<256; i++)
	{
		scr.SetColorRegister(i, r[i],g[i],b[i]);
	}

	Msg("  ��������, ���������� ��������� ...  ",1);

	for(int vc=0; vc<60; vc++) scr.wait();
	//Msg("���������� ���� ����� ������ � ����������",1);
	return 1;
}