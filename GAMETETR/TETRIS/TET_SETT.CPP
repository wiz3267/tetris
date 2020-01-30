#include <string.h>

#include "FIGURE.H"
#include "TETRIS.H"

#include "SETTINGS.H"
#include "SCREEN.H"

#include "DEBUG.H"
#include "SOUND.H"

#include "KEYBOARD.H"

#include "midpak2.h"
#include "doscall2.h"
#include "loader2.h"

#include <fstream.h>

extern KEYBOARD Keyb;

extern TETRIS Tetris[2];

int MsgText(char far *msg, char far * text1, char far * text2, int bool_init, int y_coor);

extern SETTINGS Settings;

void TETRIS::Settings1()
{
			scr.clear();

			Settings.EnableSound=MsgText("Звук", "Выкл.", "Вкл.",Settings.EnableSound);

			scr.clear();
			Settings.EnableMusic=MsgText("Музыка", "Выкл.", "Вкл.",Settings.EnableMusic);
			if (Settings.EnableMusic==0) MidiStop();

			scr.clear();
			Settings.ShowTableRecord=MsgText("Показывать табл. рекордов", "Нет", "Да", Settings.ShowTableRecord);

			scr.clear();
			Settings.ShowRunLine=MsgText("Бегущая строка", "Нет", "Да", Settings.ShowRunLine);

			scr.clear();
			Settings.ShowRealTime=MsgText("Часы реального времени", "Нет", "Да", Settings.ShowRealTime);

			//MsgText2("Бегущая строка", "Да", "Нет", &Settings.ShowRunLine);

			scr.clear();


            //??????
			//Msg(" Громкость музыки (0..99):     ",5);
			int vol=MsgTextMinMaxValue("Громкость музыки", 0, 100, Settings.MusicVolume, 100);

			Settings.MusicVolume=vol;
			SetRelativeVolume(vol,0);

			Settings.Save();

			Keyb.Set(KEY_ENTER,0);
}

void TETRIS::Settings2()
{

			scr.clear();
			Settings.EnableEffects=MsgText("Показывать эффекты ?", "Нет", "Да", Settings.EnableEffects);

			scr.clear();
			Msg(" MinumumBrickCount 1..40:    ",2);
			int nbr=scr.InputNumber(2);
			if (nbr<1) nbr=1;
			if (nbr>40) nbr=40;
			Settings.MinimumBrickCount=nbr;

			scr.clear();
			if (MsgText("Изменить размеры Тетриса ?", "Нет", "Да", 0) == 1)
			{

			scr.clear();
			Msg(" Ширина ячейки тетриса (2..9):   ",3);
			int bsz=scr.InputNumber(1);
			if (bsz>9) bsz=9;
			if (bsz<2) bsz=2;
			Settings.BoxXSize=bsz;

			scr.clear();
			Msg(" Высота ячейки тетриса (2..9):   ",3);
			bsz=scr.InputNumber(1);
			if (bsz>9) bsz=9;
			if (bsz<2) bsz=2;
			Settings.BoxYSize=bsz;

			Tetris[0].Glass.BoxLen=Settings.BoxXSize;
			Tetris[0].Glass.BoxHei=Settings.BoxYSize;

			Tetris[1].Glass.BoxLen=Settings.BoxXSize;
			Tetris[1].Glass.BoxHei=Settings.BoxYSize;

			//начисление очков
			// 1 lines (100/10) * len
			// 2 lines (300/10)
			// 3 lines (700/10)
			// 4 lines (1500/10)

			scr.clear();
			Msg(" Ширина стакана 5..30:    ",2);
			int len=scr.InputNumber(2);
			if (len<5) len=5;
			if (len>30) len=30;
			Settings.GlassLen=len;

			scr.clear();
			Msg(" Высота стакана 5..40:    ",2);
			int hei=scr.InputNumber(2);
			if (hei<5) hei=5;
			if (hei>40) hei=40;
			Settings.GlassHei=hei;

			Tetris[0].Glass.Make(len,hei);
			Tetris[1].Glass.Make(len,hei);

			scr.clear();
			Msg(" Вид ячейки тетриса (0..1):   ",3);
			Settings.BoxView=scr.InputNumber(1);

			scr.clear();
			Msg(" Вид стакана тетриса (0..1):   ",3);
			Settings.GlassView=scr.InputNumber(1);

			}

			Settings.Save();

}

//загрузить из файла
int	TETRIS::Load(char *fname)
{
	ifstream ifl;

	//пытаемся открыть файл
	ifl.open(fname, ios::binary);

	//если нет
	if (!ifl)
	{
		return 0;
	}
	else
	{

		//при загрузке!
		//ВНИМАНИЕ! в GLASS - указатель
		//ВНИМАНИЕ! в FIGURE - указатель

		TETRIS * t = new TETRIS;

		ifl.read((char*)t, sizeof(TETRIS));

		Score=t->Score;
		Lines=t->Lines;
		//SetRecord,	//был ли установлен рекорд
		Speed=t->Speed,		//скорость падения
		relScore=t->relScore,	//относительные очки
		GameMode=t->GameMode,	//режим игры
		Up=t->Up,		//кнопка вверх
		Down=t->Down,		//кнопка вниз
		Left=t->Left,		//влево
		Right=t->Right,		//вправо
		PlayerNumber=t->PlayerNumber,	//номер игрока
		Pause=t->Pause,		//пауза
		LRup=t->LRup,

		strcpy(PlayerName, t->PlayerName);


		ifl.close();
	}

	return 1;
}

//сохранить в файл
int	TETRIS::Save(char *fname)
{
	ofstream ofl;

	//пытаемся создать файл
	ofl.open(fname, ios::binary);
	if (!ofl) return 0;

	ofl.write((char*)this, sizeof(TETRIS));
	ofl.close();

	return 1;
}
