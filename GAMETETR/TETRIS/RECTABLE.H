#ifndef __RECTABLE_H
#define __RECTABLE_H

//класс RECTABLE - таблица результатов
class RECTABLE
{
public:

	enum {
		MAX_RECORD=20,		//максимальное число рекордов
		NAME_MAXLEN=14		//максимальная длина имени
	};


//хранит информацию о рекордсменах
struct RECORD
{
	char name[ NAME_MAXLEN] ;		//имя
	char color;
	unsigned long score;	        //число очков
	unsigned long lines;			//число линий
	unsigned int time	;			//время игры
};

RECORD record[MAX_RECORD+1];		//указатель на таблицу рекордов

public:
	RECTABLE();	//конструктор
	~RECTABLE();	//дестркутор
	void Save();		//записать таблицу на диск
	void Load();		//считать таблицу с диска

	//попытка поместить новый рекорд в таблицу
	//void PutNewRecord(unsigned long record, unsigned long lines);
	char * PutNewRecord(char * _name, unsigned long newscore, unsigned long lines, unsigned int _time);
	void PutNewRecordNameColor(char *name, unsigned long newscore, char Color);
	void Show(int newnum=-1);		//показать таблицу на экране

	//получить максимальный рекорд из таблицы
	unsigned long GetMaxScore();
};

#endif