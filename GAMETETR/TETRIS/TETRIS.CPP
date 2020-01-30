//игра "TETРIC". январь 1998 - декабрь 2010 г.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <fstream.h>

#include "figure.h"
#include "glass.h"
#include "tetris.h"

#include "mouse.h"
#include "screen.h"
#include "sound.h"
#include "keyboard.h"
#include "timer.h"
#include "sprite.h"
#include "rectable.h"
#include "settings.h"
#include "fontedit.h"

//=================файлы звуковых драйверов=================
#include "midpak2.h"
#include "doscall2.h"
#include "loader2.h"

extern unsigned long RunLX;

SETTINGS Settings;

void graph_draw_symbol(int x, int y, char sym);
void graph_draw_num(int x, int y, int num);

unsigned long MaxScore;		//максимальное число очков, набранное в игре
void GetRealTimeClock(unsigned char &hour, unsigned char &min, unsigned char &sec);

void DrawRunLine(int y);
float BaseSpeed=1, BaseAccelerat=1;

int CURRENT_MUSIC=0;

RECTABLE Rectable;

int KeyDownPress=0;

TETRIS Tetris[2];

extern SPRITE SpriteBox;

void music(int n, int seq);

// *********** глобальные переменные ***********
	MOUSE	Mouse(&scr);
	SOUND	Sound;
	SPRITE	Sprite(&scr,5);
	SPRITE	Background(&scr);		//содержит задние фона
	MOVEFIGURE Movefig(&SpriteBox);	//перемещающиеся фигуры
// *********************************************

//trunc - возвращает дробную часть
template <class T>
T trunc(T arg)
{
	return arg-int(arg);
}

void load_sounds()
{
	Sound.load("sound\\pop.raw");
	Sound.load("sound\\xplode2.raw");
}

//============== redfine_keys - назначение клавиш ==============
void redefine_keys()
{
	int i;
	int x=14,s=2,k=9,t=6,repeat=1;

	SCREEN &Screen=scr;

	while (repeat)
	{
		Screen.clear();

		for (i=0; i<2; i++)
		{
			Screen.setcolor(9);
			Screen.locate(x,2+i*k);

			Screen.putText("ИГРОК ");
			Screen.putNumb(i+1,1);

			Screen.setcolor(15);
			Screen.locate(x-t,s+i*k+2);
			Screen.putText("ВЛЕВО   ");
			Screen.update();
			Screen.putText( Keyb.KEY[Tetris[i].Left=Keyb.ReadScan()] );
//			Sound.play(SND_EXPLODE);

			Screen.locate(x-t, s+i*k+3);
			Screen.putText("ВПРАВО  ");
			Screen.update();
			Screen.putText( Keyb.KEY[Tetris[i].Right= Keyb.ReadScan() ] );
//			Sound.play(SND_EXPLODE);

			Screen.locate(x-t,s+i*k+4);
			Screen.putText("ВВЕРХ   ");
			Screen.update();
			Screen.putText( Keyb.KEY[Tetris[i].Up = Keyb.ReadScan() ] );

			Screen.locate(x-t,s+i*k+5);
			Screen.putText("ВНИЗ    ");
			Screen.update();
			Screen.putText( Keyb.KEY[Tetris[i].Down= Keyb.ReadScan() ] );

			Screen.locate(x-t,s+i*k+6);
			Screen.putText("ПАУЗА   ");
			Screen.update();
			Screen.putText( Keyb.KEY[Tetris[i].Pause= Keyb.ReadScan() ] );

		}

		Screen.setcolor(143+15*2);

		if (MsgText("Верно ?", "Да",  "Нет",0, 19) == 1)
		{
			continue;
		}
		else
		{
			return;
		}
	}

//	Sound.play(SND_EXORCISM, NOSTOP);
}

//деструктор TETRISa
TETRIS::~TETRIS()
{
}

void generate_picture();
//сама игра
extern SPRITE SpriteBox;

void TetrisHelp();

void TETRIS::game()
{


	if (Keyb[KEY_LEFTCTRL] && Keyb[KEY_1])
	{
		int len=Glass.Len;
		int hei=Glass.Hei;
		if (len>5) len--;
		Glass.Make(len,hei);

		Keyb.Set(KEY_1,0);
	}
	if (Keyb[KEY_LEFTCTRL] && Keyb[KEY_2])
	{
		int len=Glass.Len;
		int hei=Glass.Hei;
		if (len<40) len++;
		Glass.Make(len,hei);

		Keyb.Set(KEY_2,0);
	}

	if (Keyb[KEY_LEFTCTRL] && Keyb[KEY_3])
	{
		int len=Glass.Len;
		int hei=Glass.Hei;
		if (hei>5) hei--;
		Glass.Make(len,hei);

		Keyb.Set(KEY_3,0);
	}

	if (Keyb[KEY_LEFTCTRL] && Keyb[KEY_4])
	{
		int len=Glass.Len;
		int hei=Glass.Hei;
		if (hei<40) hei++;
		Glass.Make(len,hei);

		Keyb.Set(KEY_4,0);
	}



	if (Keyb[KEY_F1])
	{
		TetrisHelp();
	}

	generate_picture();

		//уменьшаем задержку для генерации новой фигуры
		if (NewfigureDelay) NewfigureDelay--;

		scr.setcolor(11);
		Brick.MakeSupervisor(&SpriteBox);

		//FIGURE &fg=Figure;

		//(логика)
		//если надо генерить новую фигуру и нет падающих кирпичей
		//и нет задержки
		int N=Settings.MinimumBrickCount;	//если меньше, все идет вниз

		//scr.color=15;
		//scr.bkcolor=2;
		//scr<<cur(0,0);
		//scr<<N;

		if (GameMode & GAMEMODE_NEWFIGURE && (Brick.Counter<N)

		  && !NewfigureDelay)
		{
				NewFigureGenerate();

		}

		//*********** I. построение изображение *************

		static int show_pl_name=1;
		if (Keyb[KEY_F7])
		{
			show_pl_name^=1;
			Keyb.Set(KEY_F7,0);
		}
		if (show_pl_name)
		{
			//показываем имя игрока
			scr.locateXY(Glass.x+8*1, Glass.y+Glass.BoxHei*Glass.Hei+1);
			scr.color=15;
			scr.bkcolor=0;
			//scr<<PlayerName;
		//scr<<"ИГРОК "<<PlayerNumber;

		}

		//если режим игры не генерить фигуру и нет летящих фигур
		if (~GameMode & GAMEMODE_NEWFIGURE && !Movefig.Counter)
		{
			//показываем фигуру
			Figure.Show();
		}

		//показываем задний план
		//Glass.ShowBackground(1);//Speed);

		//показываем разлетающиеся кирпичи
		Brick.Animate();

		//показываем стакан
		Glass.Show();

	if (Keyb[KEY_PAGEUP] && Keyb[KEY_LEFTSHIFT])
	{
		Msg("Введите число очков 1-ый:       ",5);
		Tetris[0].Score=scr.InputNumber(5);

		Msg("Введите число очков 2-ой:       ",5);
		Tetris[1].Score=scr.InputNumber(5);
	}


	if (Score<35000)
	{
		if (Score>30000)
		{
			Background.Scroll(9, SPRITE::SCROLL_RIGHT);
			Background.Scroll(9, SPRITE::SCROLL_UP);
		}

		if (Score>25000)
		{
			Background.Scroll(9, SPRITE::SCROLL_RIGHT);
		}
		else	if (Score>20000)
		{
			Background.Scroll(9, SPRITE::SCROLL_UP);
		}
	}

		static int show_info=1;
	//показываем как много набрали очков
	{
		int maxhei=21*8;
		int hei;

		//определяем рекорд из десятка лучших, который можем побить
		for(int t=Rectable.MAX_RECORD-1; t>=0; t--)
		{
			if (Rectable.record[t].score>Score) break;

		}

		//если очков набрали больше, чем максимльное число
		if (Score>=MaxScore)
		{
			MaxScore=Score;
			//если не был установлени рекорд
			if (!SetRecord)
			{
				SetRecord=1;

				//очищаем стакан
				/*for(int i=0; i<Glass.Hei; i++)
				for(int j=0; j<Glass.Len; j++)
				{

					//красота
					ANIM ob;
					ob.x=Glass.x+8+j*8;
					ob.y=Glass.y+i*8;
					ob.SpriteIndex=Glass.Read(j,i);
					ob.Acceleraty=0.1*random(10);
					ob.Speedx=(0.1*random(50))*BaseSpeed;
					ob.Speedy=-2-0.1*random(80);
					if (random(2)) ob.Speedx*=-1;

					if (ob.SpriteIndex)
						Brick+=ob;

					Glass.Write(j,i,0);
				  }
				  */
			}
		}

		char str[]="HI SCORE";

		//если рекорд
		if (Score>MaxScore)
		{
			for(int i=0; i<strlen(str); i++)
			{
				scr.locateXY(Glass.x+100+8, Glass.y+20+float(maxhei-40)/strlen(str)*i);
				scr.setcolor(1+random(15));
				scr<<str[i];
			}
		}
//		scr.setcolor(k);

		//!!!увеличение скорости !!!
		//Speed=Score/4000;
		Speed=Score/4000;
		if (Speed>9) Speed=9;

		//увеличиваем(если возможно) число очков
		int dec=5;
		if (relScore-dec>=0)
		{
			relScore-=dec;
			Score+=dec;
		}
		else if (relScore)
		{
			Score+=relScore;
			relScore=0;
		}

		scr.locateXY(Glass.x,Glass.y-16-8);

		if (show_info)
		{
        int dx=8;
		//вывод времени игры
		{
			unsigned int Timer=this->Timer/50;
			int min=Timer/60,sec=Timer%60;

			scr.color=22;
			scr.bkcolor=0;

			//scr<<"Время ";


			//scr<<min<<':';
			scr.color=11;
			int x=Glass.x+dx,y=Glass.y-15-8;
			if (min<10)
			{
			graph_draw_num(x, y, 0);
			graph_draw_num(x+5, y, min);
			}
			else graph_draw_num(x, y, min);

			if (sec<10)
			{
			graph_draw_num(x+15, y, 0);
			graph_draw_num(x+15+5, y, sec);
			}
			else
			{
				graph_draw_num(x+15, y, sec);
			}

			graph_draw_symbol(x+10, y, ':');

			//scr.border(0,0,320,200,2);
			//Glass.x+9*8+dx,Glass.y-8-4+dy
			//if (sec<10) scr<<'0';
			//scr<<sec;
		}

		scr.locateXY(Glass.x,Glass.y-16);
		scr.bkcolor=0;
		scr.color=23;

		//?????

		int olbk=scr.bkcolor;
		int olcl=scr.color;
		if (relScore)
		{
			static int time_score=0;
			time_score++;

			//if (time_score & 64)
			//{
			//	scr.bkcolor=9;
			//	scr.color=15;
			//}
			//else
			//	{
			//	scr.bkcolor=15;
			//	scr.color=9;
			//}
		}

		//
		//scr<<"Очки "<<(unsigned long)Score;
		//scr.locateXY(Glass.x,Glass.y-8);
		scr.color=13;
		graph_draw_num(Glass.x+dx, Glass.y-16, Score);

		scr.bkcolor=olbk;
		scr.color=olcl;

//		scr.setcolor(12);
		//scr<<"Линий "<<(unsigned long)Lines;
		scr.color=14;
		graph_draw_num(Glass.x+dx, Glass.y-8, Lines);

		//скорость игры
		int dy=-3;
		scr.setcolor(15);
		scr.bkcolor=0;
		//scr.border(Glass.x+9*8-4+1+dx,Glass.y-8-8+dy+1,13,13,3);
		//scr.locateXY(Glass.x+9*8+dx,Glass.y-8-4+dy);
		//scr<<Speed;
		scr.color=26;
		graph_draw_num(Glass.x+9*8+dx,Glass.y-8-4+dy-8, Speed);


		}
	}

		if (Keyb[KEY_F9])
		{
			show_info^=1;
			Keyb.Set(KEY_F9,0);
		}

		//если режим игры 'GAME OVER' и нажали кнопку вниз
		if (GameMode & GAMEMODE_GAMEOVER && Keyb[Down])
		{
			Timer=0;	//сбрасываем таймер

			GameMode^=GAMEMODE_GAMEOVER;

			Glass.Clear();

			Score=0;

			relScore=0;

			Lines=0;

			SetRecord=0;
		}

		//если надо выводить надпись 'GAME OVER'
		if (GameMode & GAMEMODE_GAMEOVER)
		{
			char * s="ИГРА ОКОНЧЕНА";
			scr.setcolor(12);
			int len=Glass.Len*Glass.BoxLen;
			int len2=strlen(s)*scr.StepX;
			int z=(len-len2)/2;
			scr.locateXY(Glass.x+z+8, Glass.y+80);
			scr.bkcolor=16;
			scr.setcolor(15);
			scr<<s;
			scr.bkcolor=0;
			return;
		}


		//for(int i=0; i<256; i++)
		{
		  //	scr.line_down(i,0,23, i);
		}

		if (Keyb[KEY_F1] && Keyb[KEY_LEFTCTRL])
		{
			Keyb.Set(KEY_F1,0);
			Keyb.Set(KEY_LEFTCTRL,0);

			NewFigureGenerate();

		}

		if (Keyb[Pause]==1)
		{
			Keyb.Set(Pause,2);
			GameMode^=GAMEMODE_PAUSE;
		}

		//если надо выводить надпись 'Пауза'
		if (GameMode & GAMEMODE_PAUSE)
		{
			scr.locateXY(Glass.x+16-8, Glass.y+80);
			scr.bkcolor=0;
			scr.setcolor(::Timer.GetTotalCounter()/10);
			scr<<"  ПАУЗА";
			scr.bkcolor=0;

			return;
		}
		else
		{
			Timer+=BaseSpeed;
		}

		NextFigureShow();


	//*********** II. логика **************

		//если все кирпичи упали, надо все сдвинуть вниз
		if ( (GameMode & GAMEMODE_NEWFIGURE) && Brick.Counter<N)
		{
			for(int i=0; i<Glass.Hei; i++)
			{
				int yes=1;
				for(int j=0; j<Glass.Len; j++)
				{
					if ( Glass.Read(j,i).t )
					{
						yes=0;
						break;
					}
				}

				//если надо сбросить линию
				if (yes)
				{
				  int k=0; 	//двигали ли на самом деле
				  //сдвигаем все вниз
				  for(int y=i; y>=1; y--)
				  for(int x=0; x<Glass.Len; x++)
				  {
					BOX m=Glass.Read(x,y-1);
					if (m.t) k=1;
					Glass.Write(x,y, m);
				  }

				  //обнуляем нулевую строку
				  for(int x=0; x<Glass.Len; x++)
				  {BOX b;
				  b.t=0;
				  b.sym=0;
				   Glass.Write(x,0,b);
				  }

				  if (k)
				  Sound.play(SND_POP);
				}
			}
		}

	//падающие звезды
	/*if (!random(20))
	{
		ANIM ob;
		ob.x=random(320);
		ob.y=random(200);
		ob.SpriteIndex=SPRITE_TEST;
		ob.AlwaysAnimate=1;
		Brick+=ob;
	}*/

	 //если не надо генерить новую фигуру - можно двигать старой
	 if (~GameMode & GAMEMODE_NEWFIGURE)
	 {

		//стираем фигуру
		Figure.Show(0);


//		int count_busy=0;
		int count_free=0;
		//проверка свободных линий
		for(int j=0; j<Glass.Hei; j++)
		{
			for(int x=0; x<Glass.Len; x++)
			{
				if (Glass.Read(x,j).t) break;
			}
			if (x==Glass.Len) count_free++;
			else break;
		}

		//scr.locate(0,6);
		//scr<<"free="<<count_free;

		//?????

		if (PlayerNumber==0)
		{
		if ( (count_free <7) && (CURRENT_MUSIC!=0))
		{
			CURRENT_MUSIC=0;
			MidiStop();
		}

		if ( (count_free >12) && (CURRENT_MUSIC != 4))
		{
			CURRENT_MUSIC=4;
			MidiStop();
		}
		}
		float Speedy=(0.03+Speed/120.0)*BaseSpeed;

		if (Keyb[Down] ==1) Speedy=0.5*BaseSpeed;

		if (Keyb[Down] ==1 && Keyb[KEY_RIGHTSHIFT])
		{
			KeyDownPress=1;
			Speedy=1;
		}

		if (Speedy>1) Speedy=1;

		//пытаемся двинуть фигуру вниз
		{
			//если нет летящих фигур
			if (!Movefig.Counter)
				Figure.y+=Speedy;

			//если двигаться вниз нельзя
			if (!Figure.Check())
			{
				//показываем фигуру
				Figure.y--;
				Figure.Show();

				//даем команду на генерацию новой фигуры
				GameMode|=GAMEMODE_NEWFIGURE;

				NewfigureDelay=8/BaseSpeed;


		//сбрасываем линии ( это надо сделать красивее ! )
		{
			int t=0;
			//int m=random(256);
			int line_full=0;

			for(int i=0; i<Glass.Hei; i++)
			{
				int yes=1;
				for(int j=0; j<Glass.Len; j++)
				{
					if ( !Glass.Read(j,i).t )
					{
						yes=0;
						break;
					}
				}

				//если надо сбросить линию
				if (yes)
				{
				  Lines++;
				  line_full++;
				  //красота
				  for(int x=0; x<Glass.Len; x++)
				  {
					ANIM ob;

					ob.x=Glass.x+8+x*Glass.BoxLen;
					ob.y=Glass.y+i*Glass.BoxHei;

					ob.SpriteIndex=Glass.Read(x,i).sym;
					ob.AlwaysAnimate=1;
					ob.Color=15;//ob.SpriteIndex;
					//ob.symbol=Glass.

					ob.BoxLen=Glass.BoxLen;
					ob.BoxHei=Glass.BoxHei;

					ob.Speedy=(line_full * -0.1 -0.01*random(20))*BaseSpeed;
					ob.Speedx=(0.2+0.1*(x-5)*(line_full))*BaseSpeed;

					ob.Acceleraty=-(0.01+0.001*random(50))*BaseAccelerat;

					Brick+=ob;

				  }

				  //обнуляем эту линию
				  for(x=0; x<Glass.Len; x++)
				  {
					BOX b={0,0};
					Glass.Write(x,i, b);
				  }

					t++;
					Sound.play(SND_XPLODE, NOSTOP);
				}
			}
			int glen=Glass.Len;
			switch(t)
			{

				case 1:
				relScore += 10*glen;
				break;

				case 2:
				relScore += 30*glen;
				break;

				case 3:
				relScore += 70*glen;
				break;

				case 4:
				relScore += 150*glen;
				break;
			}
		}


			}
		}

		float Speed_x=(0.2+Speed/80.0)*BaseSpeed;

		if (Speed_x>1) Speed_x=1;

		//Speed_x=1;
		if (Keyb[KEY_LEFTSHIFT]) Speed_x=1;

		if ( Keyb[Left] == 1 &&	~GameMode & GAMEMODE_NEWFIGURE	)
		{
			if (Keyb[KEY_LEFTSHIFT])
			{
				Keyb.Set(Left,0);
			}

			//Keyb.Set(Left,0);

			//fg.Show(0);

			Figure.x -= Speed_x;
			//fg.x=int(fg.x);
			//fg.x--;

			if (Figure.x<0) Figure.x=0;

			if ( !Figure.Check() )
			{
				Figure.x+=Speed_x;
				//fg.x++;
				Figure.Show();
			}
		}

		/*scr.locate(0,2);
		scr<<fg.x;
		scr.locate(0,3);
		scr<<"sx=" << Speed_x;
		scr.locate(0,4);
		scr<<"sy=" << Speedy;
		scr.locate(0,5);
		scr<<"bs=" << BaseSpeed;
		*/

		// <RIGHT> пытаемся двинуться вправо
		if (Keyb[Right] == 1 && ~GameMode & GAMEMODE_NEWFIGURE)
		{
			if (Keyb[KEY_LEFTSHIFT])
			{
				Keyb.Set(Right,0);
            }

			Figure.Show(0);

			Figure.x += Speed_x;

			//fg.x=int(fg.x);
			//fg.x++;

			if ( !Figure.Check() )
			{
				Figure.x -= Speed_x;
				//fg.x--;
				Figure.Show();
			}
		}

		//если не нажата ВЛЕВО и ВПРАВО
		if (!Keyb[Left] && !Keyb[Right])
		{
			LRup=1;

			//float drob=fg.x-int(fg.x);

			//fg.x=int(fg.x);

			//if (drob>0.0 && drob<0.3) fg.x++;
		}


		/*if (Keyb[Up] && Keyb[Right] && Keyb[Left])
		{
				//очищаем стакан
				for(int i=0; i<Glass.Hei; i++)
				for(int j=0; j<Glass.Len; j++)
				{

					//красота
					for(int z=0; z<3;z++)
					{
					ANIM ob;
					ob.x=Glass.x+8+j*8;
					ob.y=Glass.y+i*8;
					ob.SpriteIndex=Glass.Read(j,i);
					if (random(10)) ob.Acceleraty=0.1;
					else ob.Acceleraty=0.3;
					ob.Speedx=(0.1*random(100))*BaseSpeed;
					ob.Speedy=-1-0.1*random(100);
					if (random(2)) ob.Speedx*=-1;

					if (ob.SpriteIndex)
						Brick+=ob;
					}

					Glass.Write(j,i,0);
				  }
		}
		*/

		// <UP> пытаемся изменить вид фигуры
		if (Keyb[Up] == 1 && ~GameMode & GAMEMODE_NEWFIGURE)
		{
			Keyb.Set(Up,2);

			Figure.Show(0);	//стираем фигуру

			Figure.Aspect++;	//меняем ее вид

			int d=0;	//на сколько сместили фигуру

			//если фигура не помещается
			if (!Figure.Check())
			{
				for(int i=0; i<3; i++)
				{
					d++;
					Figure.x--;
					if (Figure.Check())
					{
						break;
					}
				}
				if (i==3)
				{
					Figure.Aspect--;
					Figure.x+=d;
				}

			}
			//иначе показываем фигуру
			else
			{
				Figure.Show();
			}
		}
	 }


}

char	*song[10];
long	song_len[10];

void music(int n, int seq)
{
	if (Settings.EnableMusic==0) return;

	RegisterXmidi(song[n],song_len[n]); // register the song.
	//MidiStop();
	PlaySequence(seq);
	SetRelativeVolume(Settings.MusicVolume,0);

}

void load_music()
{
	char *music[]={
			"music\\0.xmi",
			"music\\1.xmi",
			"music\\2.xmi",
			"music\\3.xmi",
			"music\\4.xmi",
			"music\\5.xmi",
			"music\\6.xmi",
	};

	for(int i=0; i<6; i++)
		song[i] = fload( music[i],  &song_len[i]); // Load song into memory.
}

void ProgrammEditor()
{
	ifstream ifl;

	ifl.open("TETRIS.CPP");
	if (!ifl) return;

	scr.clear(9);
    scr.rectangle(0,0,320,200, 9);
	scr.x=0;
	scr.y=0;

	scr.bkcolor=9;
	scr.color=15;

	while (!ifl.eof())
	{
		unsigned char buf[512];

		ifl.getline(buf,510);
		for(int i=0; i<strlen(buf); i++)
		{
			if (buf[i]<32) buf[i]=32;
		}

		for(i=0; i<strlen(buf); i++)
		{
			scr<<char(buf[i]);
			if (scr.x> (320-8) ) {scr.x=0, scr.y+=8;}
		}
        scr<<"\n";

		scr.update();
		if (scr.y>180)
		{
			if (Keyb.ReadScan(3276700L) == KEY_ESC) break;
			scr.x=0;
			scr.y=0;
			scr.rectangle(0,0,320,200, 9);
			//scr.clear(9);
		}
	}

	ifl.close();
}

int TetrisStart();

extern int BigSymScale;

void main()
{
	MOVEOBJ::Lim.Set(-24,-80,320+8,208);
	//загружаем данные
	{
		load_sprites();
		load_sounds();
		load_music();
		Rectable.Load();
		Settings.Load();
	}
	scr.StepX=6;
	BigSymScale=Settings.BigSymScale;

	music(0,0);
    
	//if (!TetrisStart()) return;

	randomize();

	Tetris[0].Load("tetris0.ttr");
	Tetris[0].Glass.Load("tetris0.gls");


	Tetris[1].Load("tetris1.ttr");
	Tetris[1].Glass.Load("tetris1.gls");

	MaxScore=Rectable.GetMaxScore();

	int dx=10;

	Tetris[0].Figure.Glass=&Tetris[0].Glass;

	Tetris[0].Figure.x=2;
	Tetris[0].Figure.y=2;
	Tetris[0].Figure.Number=random(4);
	Tetris[0].Figure.Color=1+random(14);

	Tetris[0].Pause=KEY_P;
	Tetris[0].PlayerNumber=0;

	//инициализируем первого игрока
	{
		randomize();
		TETRIS &tr=Tetris[1];
		tr.Figure.Glass=&tr.Glass;

		tr.Figure.x=2;
		tr.Figure.y=2;
		tr.Figure.Number=random(4);
		tr.Figure.Color=1+random(14);

		tr.Up=KEY_S;
		tr.Down=KEY_X;
		tr.Left=KEY_Z;
		tr.Right=KEY_C;
		tr.PlayerNumber=2;
		tr.Pause=KEY_A;
		tr.PlayerNumber=1;

//		tr.GameMode = TETRIS::GAMEMODE_GAMEOVER;
	}

restart:

	int k;

	strcpy(Tetris[0].PlayerName, "1 игрок");
	strcpy(Tetris[1].PlayerName, "2 игрок");

	for(int ff=0; ff<10; ff++)
	{
	Tetris[0].NewFigureGenerate();
	Tetris[1].NewFigureGenerate();
	}

	Timer.Restart(50);
	do
	{
		RunLX=0;
		k=Tetris[0].menu();

		if (k==2) redefine_keys();

		if (k==3) Tetris[0].Settings1();
		if (k==4) Tetris[0].Settings2();

		if (k==5)
		{
			Rectable.Show(0);
			scr.update();
			while (!Keyb[KEY_ESC])
			{
			}
		}

		if (k==6)
		{
			Tetris[0].Figure.Show(0);
			Tetris[0].Glass.Save("tetris0.gls");
			Tetris[0].Save("tetris0.ttr");

			Tetris[1].Figure.Show(0);
			Tetris[1].Glass.Save("tetris1.gls");
			Tetris[1].Save("tetris1.ttr");

			return;
		}

		if (k==7)
		{
			FontEditor(0);
			//ProgrammEditor();
		}

		if (k==8) TetrisHelp();

		if (k==9)
		{
			TetrisStart();
			Timer.Restart(50);
		}

	}while (k>1);

	//запускаем таймер на частоту 50 Гц
	Timer.Restart(50);

	//изменяем последовательность случайных чисел
	{
		randomize();
	}

	int GAME=1;

	MidiStop();

	if (k==0 || k==1)
	{
		MidiStop();
		music(CURRENT_MUSIC,0);
	}

	if (k==1)
	{
		scr.clear();

		Keyb.Clear();

		static int first_run=1;

		if (first_run)
		{
			first_run=0;

			//if (Tetris[0].PlayerName[0]==0)
			{
				//Msg(" 1 ИГРОК ИМЯ:                  ",9);
				//scr.InputString(15, Tetris[0].PlayerName);
			}

			//if (Tetris[1].PlayerName[0]==0)
			{
				//Msg(" 2 ИГРОК ИМЯ:                  ",9);
				//scr.InputString(15, Tetris[1].PlayerName);
			}
		}
	}

	while ( GAME)
	{

		//очистка экрана
		scr.clear();

		//режим двух игроков
		if (k==1)
		{

			Tetris[0].Glass.x=24*8-dx+16-4;
			Tetris[0].Glass.y=24;

			if (Tetris[0].Glass.BoxHei>8) Tetris[0].Glass.y=0;

			Tetris[1].Glass.x=2;
			Tetris[1].Glass.y=24;

			if (Tetris[1].Glass.BoxHei>8) Tetris[1].Glass.y=0;

			Tetris[0].game();
			Tetris[1].game();

			static int Show_Score_Counter=0, compare=0;

			long a=Tetris[0].Score, b=Tetris[1].Score;

			// 100 0

			if (compare>0 && a<b
					||
				compare<0 && a>b
					||
				!compare && a!=b)
			{
				Show_Score_Counter=180;
				compare=a-b;
			}

		}

		if (k==0)
		{
			Tetris[0].Glass.x=15*8-dx;
			Tetris[0].Glass.y=24;

			if (Tetris[0].Glass.BoxHei>8) Tetris[0].Glass.y=0;

			Tetris[0].game();
		}

		//int first_record=0;

	if (Keyb[KEY_F11])
	{
		Settings.ShowTableRecord^=1;
		Settings.Save();
		Keyb.Set(KEY_F11,0);
	}


	int x_record=0;
	if (k==0)
	{
		x_record=2;
	}
	else
	{
		x_record=100-8+30;
	}

	if (Settings.ShowTableRecord)
	{

	  RECTABLE rectable_new=Rectable;

	  rectable_new.PutNewRecordNameColor(Tetris[0].PlayerName, Tetris[0].Score,9);
	  rectable_new.PutNewRecordNameColor(Tetris[1].PlayerName, Tetris[1].Score,12);

		for(int i=0; i<rectable_new.MAX_RECORD-1; i++)
		{
			scr.bkcolor=rectable_new.record[i].color;

			scr.locateXY(x_record,(i+1)*8+23);

			if (rectable_new.record[i].color!=0)
			{
				scr.color=15;
			}
			else scr.color=25;

			if (i<9)
			{
				scr<<i+1;
			}
			else
			{
				char c[2]={'A'+i-9,0};
				scr<<c;
			}

			scr<<" ";


			int flag_end=0;

			for(int j=0; j<5; j++)
			{
				char a=rectable_new.record[i].name[j];

				if (a==0) flag_end=1;

				if (!flag_end)	scr<<a;
				else scr<<" ";

			}

			scr<<" ";

			scr.putNumb(rectable_new.record[i].score,5);
		}
	}

	//scr.locate(0,5);
	//scr<<CURRENT_MUSIC;

	if (Keyb[KEY_F10])
	{
		Settings.ShowRealTime^=1;
		Settings.Save();
		Keyb.Set(KEY_F10,0);
	}

	if (Settings.ShowRealTime)
	{

		unsigned char hour, min, sec;
		GetRealTimeClock(hour, min, sec);
		char buf[32];

		sprintf(buf,"%02i:%02i:%02i", hour, min, sec);

		scr.color=9;
		scr.locateXY(x_record+20, 184);
		scr<<buf;
	}


		if (Keyb[KEY_TAB])
		{
			dx=dx++ % 200;
			static int x=0;

			x++;
			if (x>4) x=0;

			//scr.pause(100);
		}

		if ( (SequenceStatus() != SEQ_PLAYING) )
		{
			music(CURRENT_MUSIC,0);
		}

		//Mouse.Draw();
		//scr.setcolor(7);
		//scr<<cur(8,24)<<"(C) 1997 Lapshin Ewgeny";

		Movefig.Animate();	//показываем двигающиеся фигуры

		//Sprite.Show(130,194, SPRITE_AUTORNAME);

		//вычисляем новую базовую скорость
		if (Timer.Counter && Timer.Freq>19)
		{
			//Timer.Restart(Timer.Freq-1);

			/*float n=50.0/Timer.Freq;

			BaseSpeed=n;
			BaseAccelerat=n*n;

			//добавлено 11 декабря 2010
			BaseSpeed=1;
			*/
			scr<<cur(0,0)<<"?";

		}

		//синхронизируемся по таймеру
		while ( !Timer.Counter )
		{
		}

		Timer.Counter=0;

		//scr.border(0,0,320,200,15);
		if (Keyb[KEY_F12])
		{
			Settings.ShowRunLine^=1;
			Settings.Save();
			Keyb.Set(KEY_F12,0);
		}

		if (Settings.ShowRunLine)
		{
			BigSymScale=Settings.BigSymScale;
			DrawRunLine(200-BigSymScale*8);
		}

		scr.update();

		if (Keyb[KEY_F4])
		{
				Msg("MinumumBrickCount:    ",2);
				Settings.MinimumBrickCount=scr.InputNumber(2);
				Settings.Save();
		}


		//если общая пауза
		if (Keyb[KEY_PAUSE])
		{
			Msg(" PAUSE ",1);
			Keyb.ReadScan();
			//чтобы не было ложного срабатывания снижения частоты
			Timer.Counter=0;
		}

		if (Keyb[KEY_F2])
		{
			if (CURRENT_MUSIC++ >7) CURRENT_MUSIC=0;
            Keyb.Set(KEY_F2,0);

			MidiStop();

		}
		extern int StepRunLX;
		extern int show_debug;
		extern int need_wait;
		extern int music_loop;

	if (Keyb[KEY_Y]) show_debug^=1, Keyb.Set(KEY_Y,0);
	if (Keyb[KEY_Q]) StepRunLX--, Keyb.Set(KEY_Q,0);
	if (Keyb[KEY_W]) StepRunLX++, Keyb.Set(KEY_W,0);
	if (Keyb[KEY_E]) need_wait^=1, Keyb.Set(KEY_E,0);
	if (Keyb[KEY_U]) music_loop^=1, Keyb.Set(KEY_U,0);


		if (Keyb[KEY_ESC])
		{
			//scr.setcolor(14);
			//if (MsgText("Выход ?", "Да", "Нет", 1) == 0)
			{
				GAME=0;
			}
		}

	}
	goto restart;
}