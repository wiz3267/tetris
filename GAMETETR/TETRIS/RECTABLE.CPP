#include <conio.h>
#include <stdio.h>
#include <fstream.h>

#include "screen.h"
#include "debug.h"
#include "keyboard.h"
#include "rectable.h"
#include "string.h"
#include "stdlib.h"
#include "timer.h"

//***********************************************************************
//********* RECTABLE.CPP - программы для работы с таблицей рекордов *****


//показать таблицу
void RECTABLE::Show(int newnum)
{

	for(long j=0; j<16*20ul; j++)
	{
		scr.clear();
		//вывод таблицы на экран
		int x=(40-16-NAME_MAXLEN)/2-1,
		y=28;//+(25-MAX_RECORD*2)/2;

/*		scr.rectangle(x*8-8,y*8-8,
			(11+NAME_MAXLEN)*8+8*7,
			(MAX_RECORD*2+2)*8,
			143+16*4-5);*/

		//int hei=(MAX_RECORD*2)*8, len=(11+NAME_MAXLEN)*8+8*7;
		//st_x=x*8-8, st_y=y*8-8+1;

		//for(int i=0; i<hei; i++)
		// scr.line(st_x,st_y+i, st_x+len,st_y+i, 144+i%16);

		scr.color=15;

		for (int i=0; i<MAX_RECORD; i++)
		{
			//рисуем синим, если это строка того рекорда
				int hei=8, len=(18+NAME_MAXLEN)*8,
				st_x=x*8-8, st_y=(y)+i*8-6-1;

			if (i==newnum)
			{
				for(int z=0; z<hei; z++)
				 scr.line(st_x,st_y+z, st_x+len,st_y+z, 144+16*3+z%16);
/*
				scr.rectangle(
					x*8-8,
					(y+i*2)*8-5,
					(18+NAME_MAXLEN)*8+2,
					14,
					144+16*3+3+j%13);*/
			}
			scr.locateXY(st_x,st_y);
			if (i<9)
			{
			scr.putNumb(i+1,1);
			}
			else
			{
			char c[]={'A'+i-9,0};
			scr<<c;
			}
			scr.putSymbol(' ');
			record[i].name[NAME_MAXLEN-1]=0;

			char zx[8];

			scr<<record[i].name;
			scr.locateXY(st_x+NAME_MAXLEN*8+4, st_y);

			//если меньше, то 5 знаков
			if (record[i].score<65535L)
				scr.putNumb(record[i].score,5);
			//иначе сколько есть
			else
				scr.putNumb(record[i].score,5);

			scr<<" | ";
			scr.putNumb(record[i].lines,3);

			scr<<" ";

			int t=record[i].time;

			sprintf(zx,"%02i:%02i", t/60, t%60);

			scr<<zx;
		}
		scr.update();

		while (!::Timer.Counter){}
		::Timer.Counter=0;

		if (Keyb[KEY_ESC]) break;
	}
}

//считать информацию с диска
void RECTABLE::Load()
{
	for(int i=0; i<MAX_RECORD; i++)
	{
		record[i].color=0;
		record[i].lines=0;
		record[i].time=0;
		record[i].score=0;
		strcpy(record[i].name, "");
	}


	ifstream ifl;

	//пытаемся открыть файл с таблицей
	ifl.open("records.dat", ios::binary);

	char *name[]={ "Женя", "Алексей", "Папа", "Олег",
	"Жуков", "Семен"};

	//если таблица затерта, обнуляем то, что есть
	if (!ifl)
	{
		for(int i=0; i<MAX_RECORD; i++)
		{
			strcpy(record[i].name, name[random(6)]);
			record[i].score=(9-i)*500;
			record[i].lines=record[i].score/150;
			record[i].time=(MAX_RECORD-i+1)*78;
			record[i].color=2;
		}
		//сохраняем ее на диске
		Save();
		return;
	}
	//иначе читаем таблицу в память
	else
	{
		int k=sizeof(RECORD)*MAX_RECORD;
		ifl.read( (char far*) record, k );
		//закрываем файл
		ifl.close();
	}


}

//записать таблицу на диск
void RECTABLE::Save()
{
	ofstream ofl;

	//пытаемся открыть файл с таблицей
	ofl.open("records.dat", ios::binary);

	//если таблица затерта, обнуляем то, что есть
	if (!ofl)
	{
		Debug("RECTABLE::Save - can't save table to disk");
		return;
	}
	//иначе пишем таблицу на диск
	else
	{
		ofl.write( (char*) record, sizeof(RECORD)*MAX_RECORD );

		//закрываем файл
		ofl.close();
	}

}

//конструктор
RECTABLE::RECTABLE()
{
	//считываем таблицу с диска
	//Load();
}

//деструктор
RECTABLE::~RECTABLE()
{
	//сохраняем информация на диске
	//Save();
}

unsigned long RECTABLE::GetMaxScore()
{
	return record[0].score;
}

//============== попытаться положить новые очки =============
void RECTABLE::PutNewRecordNameColor(char *name, unsigned long newscore, char Color)
{
	//узнаем, вошел ли рекорд в таблицу
	if (newscore>record[MAX_RECORD-1].score)
	{
		strcpy(record[MAX_RECORD].name, name);
		record[MAX_RECORD].score=newscore;
		record[MAX_RECORD].color=Color;
		//record[MAX_RECORD].lines=lines;
		//record[MAX_RECORD].time=_time;

		//сортировка массива по возрастанию
		for (int i=0; i<=MAX_RECORD; i++)
		{
			for (int k=0; k<=MAX_RECORD-1; k++)
			{
				if (record[k].score<record[k+1].score)
				{
					RECORD tmp=record[k];
					record[k]=record[k+1];
					record[k+1]=tmp;
				}
			}
		}
	}
}


//============== попытаться положить новые очки =============
char * RECTABLE::PutNewRecord(char * _name, unsigned long newscore, unsigned long lines, unsigned int _time)
{
	//узнаем, вошел ли рекорд в таблицу
	if (newscore>record[MAX_RECORD-1].score)
	{

		Msg("Ваше имя:                 ",9);

		char player_name[NAME_MAXLEN];

		scr.InputString(
			NAME_MAXLEN, player_name);

		if (strlen(player_name)>0)
		{
			strcpy(record[MAX_RECORD].name, player_name);
		}
		else
		{
			strcpy(record[MAX_RECORD].name, _name);
		}

		record[MAX_RECORD].score=newscore;
		record[MAX_RECORD].lines=lines;
		record[MAX_RECORD].time=_time;

		scr.setbkcolor(0);
		scr.update();

		int newnum;

		//определим номер, под каким рекорд вошел в таблицу
		for (int i=0; i<MAX_RECORD; i++)
		{
			if (record[i].score<newscore)
			{
				newnum=i;
				break;
			}
		}

		//сортировка массива по возрастанию
		for (i=0; i<=MAX_RECORD; i++)
		{
			for (int k=0; k<=MAX_RECORD-1; k++)
			{
				if (record[k].score<record[k+1].score)
				{
					RECORD tmp=record[k];
					record[k]=record[k+1];
					record[k+1]=tmp;
				}
			}
		}

		Show(newnum);
		scr.update();

		//записываем
		Save();


		//scr.pause(140);

		//отменяем стандартный обработчик прерываний клавиатуры
		Keyb.SetInputMode(KEYBOARD_NOOLDINTERRUPT);

		//ждем нажатия клавиши
		Keyb.ReadScan();

		return record[newnum].name;
	}

	return NULL;
}

