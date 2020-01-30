#ifndef __TETRIS_H
#define __TETRIS_H

#include "moveobj.h"

enum {
	FGSHOW_X=150,
	FGSHOW_Y=100
};

void load_sprites();

enum {
	SPRITE_BORDER=0x200,
	SPRITE_AUTORNAME,
	SPRITE_POINTER,
	SPRITE_TEST,
	SPRITE_STAR
};

//класс MOVEFIGURE - перемещающияся фигура
class MOVEFIGURE : public ANIM
{
public:

	MOVEFIGURE(SPRITE *spr) : ANIM(spr) {}

	MOVEFIGURE() {}

	FIGURE Fig;

	void operator+=(MOVEFIGURE &ob) { New(&ob,sizeof(ob));}

	virtual	void	Show()
	{
		Fig.Show(x,y);
	}
};


//класс TETRIS
class TETRIS
{
public:

	enum { SND_POP, SND_XPLODE };

	enum {
		GAMEMODE_GAMEOVER=0x01,		//выдавать одноименную надпись
		GAMEMODE_PAUSE=0x02,		//режим паузы
		GAMEMODE_NEWFIGURE=0x04		//генерить новую фигуру
	};

	GLASS Glass;	//стакан
	FIGURE	Figure;

	float Timer;

	long Score, Lines;

	int
		SetRecord,	//был ли установлен рекорд
		Speed,		//скорость падения
		relScore,	//относительные очки
		GameMode,	//режим игры
		Up,		//кнопка вверх
		Down,		//кнопка вниз
		Left,		//влево
		Right,		//вправо
		NewfigureDelay,	//задержка для генерации новой фигуры
		PlayerNumber,	//номер игрока
		Pause,		//пауза
		LRup
	;

	char PlayerName[64];

	ANIM Brick;

public:

	int Save(char *fname);	//сохранить состояние
	int Load(char *fname);	//загрузить состояние

	void game();	//запуск игры

	int  menu();	//меню

	TETRIS();	//конструктор

	~TETRIS();	//деструктор

	void NewFigureGenerate();
	void NextFigureShow();
	void Settings1();
	void Settings2();

	unsigned char GenerateFigureColor();

};

#endif