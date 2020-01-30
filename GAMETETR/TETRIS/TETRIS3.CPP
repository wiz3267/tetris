#include "figure.h"
#include "glass.h"
#include "tetris.H"

#include "sound.h"
#include "rectable.h"
#include "screen.h"
#include "keyboard.h"
#include "settings.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>



const int NFLC=5;	//Next figure list counter
FIGURE NextFigureList[NFLC];

extern SETTINGS Settings;
extern SOUND Sound;
extern RECTABLE Rectable;
extern SCREEN scr;

void TETRIS::NewFigureGenerate()
{
	GameMode^=GAMEMODE_NEWFIGURE;

	FIGURE &fg = Figure;

	//£•≠•‡®‡„•¨ ≠Æ¢„Ó ‰®£„‡„
	fg=NextFigureList[0];
	fg.Glass=&Glass;

	for(int i=0; i<NFLC-1; i++)
	{
		NextFigureList[i]=NextFigureList[i+1];
	}

	FIGURE nf;

	nf.y=0;
	nf.x=4;
	nf.Color=GenerateFigureColor();

	nf.Number=random(fg.Counter);
	nf.Glass=&Glass;

	char glasn[27]="ÄìÖéùûàüÄÄÖìééÄùùùûéééÖÖÖìâ";
	char sogls[41]="ñäääççÉòôáááïöÜÜÜÑÑÑããêêèèÇÇÇÇîóååååëëíÅú";

	char symbols[64];
	for(int p=0; p<64; p+=2)
	{
		 symbols[p]=glasn[random(sizeof(glasn))];
		 symbols[p+1]=sogls[random(sizeof(sogls))];
	}


	nf.Aspect=0;
	nf.RandomSym(symbols);

	nf.Aspect=1;
	nf.RandomSym(symbols);

	nf.Aspect=2;
	nf.RandomSym(symbols);

	nf.Aspect=3;
	nf.RandomSym(symbols);

	nf.Aspect=random(4);


	NextFigureList[NFLC-1]=nf;

	Sound.play(SND_POP);

	//•·´® game over
	if (!fg.Check())
	{
		//Glass.Clear();
		//Score=0;
		GameMode=GAMEMODE_GAMEOVER;

		char *name=Rectable.PutNewRecord(PlayerName, Score,Lines, Timer/50);
		if (name != NULL)
		{
			strcpy(PlayerName, name);
		}
		Rectable.Save();
	}
}

void TETRIS::NextFigureShow()
{
	int x,y;

	int x_offset=10;

	if (PlayerNumber==0)
	{
		x=Glass.x+(Glass.Len)*(Glass.BoxLen)+x_offset;
		y=Glass.y;
	}
	else if (PlayerNumber==1)
	{
		x=Glass.x+(Glass.Len)*(Glass.BoxLen)+x_offset;
		y=Glass.y;
	}

	for(int i=0; i<NFLC; i++)
	{
		//int l=Glass.BoxLen*4;
		int h=Glass.BoxHei*4;
		//scr.line_right(x, y-1 + i*h, l+1, 21);
		//NextFigureList[i].BoxLen=8;
		//NextFigureList[i].BoxHei=8;
		NextFigureList[i].Glass=&Glass;
		NextFigureList[i].Show(x,y+i*h);
	}

	if (Keyb[KEY_LEFTCTRL] && Keyb[KEY_PADADD])
	{
		Settings.BoxXSize++;

		if (Settings.BoxXSize>9) Settings.BoxXSize=9;

		Settings.Save();

		Keyb.Set(KEY_PADADD,0);
	}

	if (Keyb[KEY_LEFTCTRL] && Keyb[KEY_PADSUB])
	{
		Settings.BoxXSize--;
		if (Settings.BoxXSize<2) Settings.BoxXSize=2;

		Settings.Save();

		Keyb.Set(KEY_PADSUB,0);
	}

	if (Keyb[KEY_LEFTSHIFT] && Keyb[KEY_PADADD])
	{
		Settings.BoxYSize++;

		if (Settings.BoxYSize>9) Settings.BoxYSize=9;

		Settings.Save();

		Keyb.Set(KEY_PADADD,0);
	}

	if (Keyb[KEY_LEFTSHIFT] && Keyb[KEY_PADSUB])
	{
		Settings.BoxYSize--;
		if (Settings.BoxYSize<2) Settings.BoxYSize=2;

		Settings.Save();

		Keyb.Set(KEY_PADSUB,0);
	}

	Glass.BoxLen=Settings.BoxXSize;
	Glass.BoxHei=Settings.BoxYSize;

}

extern SETTINGS Settings;

//™Æ≠·‚‡„™‚Æ‡ TETRISa ØÆ „¨Æ´Á†≠®Ó
TETRIS::TETRIS() :
	NewfigureDelay(0),
	Speed(0),
	Timer(0),
	Score(0),
	GameMode(0),
	Lines(0),
	Left(KEY_LEFT),	Up(KEY_UP),	Down(KEY_DOWN),	Right(KEY_RIGHT),
	SetRecord(0),
	relScore(0)
{
	randomize();
	PlayerName[0]=0;

	Settings.Load();

	Glass.Make(Settings.GlassLen,Settings.GlassHei);

	Glass.BoxLen=Settings.BoxXSize;
	Glass.BoxHei=Settings.BoxYSize;

	for(int i=0; i<NFLC; i++)
	{
		FIGURE &f=NextFigureList[i];
		f.y=0;
		f.x=4;
		f.Color=GenerateFigureColor();
		f.Aspect=random(4);
		f.Number=random(FIGURE::Counter);
		f.Glass=&Glass;
	}
}

unsigned char TETRIS::GenerateFigureColor()
{
	unsigned char FIGURE_COLOR=7;

	FIGURE_COLOR=random(27+(101-27));

	if (FIGURE_COLOR==0) FIGURE_COLOR++;

	if (FIGURE_COLOR>=16 && (FIGURE_COLOR<= (16+16)) ) FIGURE_COLOR+=16;

	return FIGURE_COLOR;

	/*
	unsigned char color;

	color=random(256);

	if (color==0) color++;

	if ( (color>=16) && (color<=(16+10)) ) color+=16;

	return color;
	*/
}



void generate_picture()
{

	if (Keyb[KEY_F5])
	{
		Keyb.Set(KEY_F5, 0);
		Settings.EnableEffects^=1;
		Settings.Save();

	}

	if (!Settings.EnableEffects) return;

	const int MAX_P=20;

	struct POI
	{
		int x,y,c;
	};

	static POI P[MAX_P];

	static char first=0;
	if (!first)
	{
		first=1;
		srand(0);
		for(int i=0; i<MAX_P; i++)
		{
			P[i].x=random(320);
			P[i].y=random(200);
			P[i].c=16+random(10);
		}
	}

	static float as=0;
	int x=320/2+50*sin(as), y=200/2+50*cos(as);

	for(int i=0; i<MAX_P; i++)
	{
		scr.line(x, y,
		 P[i].x, P[i].y, P[i].c);
	}

	as+=0.01;
}
