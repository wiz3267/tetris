#ifndef __SCREEN_H
#define __SCREEN_H

#include "rects.h"

class FONT
{
	//возможные перечеслиния размеров фонта
	enum {
		FONT8x8,
		FONT8x14,
		FONT8x16
	};

	char * FontAddres;		//адрес фонта
	char SymHigh;			//высота одного символа

public:
	FONT(int size=FONT8x8); //конструктор size - размер фонта
	~FONT();  	//деструктор

	//показать на экране символ с дополнительным цветом color
void	Show(int x,int y, int symbol, char color);
};

//=============== описание класса 'SCREEN'  - экран ========================
//  Примечание: такие понятия, как 'вывод на экран', 'очистка экрана'
// относятся не к физической VRAM, а к теневому экрану (doublebuffer)
// Чтобы информация появилась на экране необходимо вызвать функцию update
class SCREEN {

// ********************** ДАННЫЕ-ЧЛЕНЫ ******************************
public:

	enum
	{
		DOUBLEBUF=1,
		NODOUBLEBUF=0,
		//значение флага для функции putBigSym
		RANDOM_COLOR=0x01,
		ONE_COLOR=0x02
	};

static	char	constructor_run;//был ли запущен конструктор
static	int	CountObjects;	//число обьектов типа SCREEN
static	char	*DoubleBuffer;	//адрес теневого экрана
static	char	*VideoMemory;	//адрес физической видео памяти

public:

	char	bkcolor;		//цвет заднего фона
	char 	color;			//цвет рисования
	int	x,y;				//координаты курсора
	int StepX;			//шаг приращения при выводе символов

// ********************** ФУНКЦИИ-ЧЛЕНЫ *************************
public:
		//значение State для bitblt
	enum {
		FLIP_HORZ=0x01,	//перевернуть спрайт по горизонтали
		FLIP_VERT=0x02,	//перевернуть спрайт по вертикали
		TRANSP=0x04	//есть прозрачные пикселы
	};

	enum {
		ONLY_DIGIT=0x01	//фунция InputString будет вводить только цифры
	};

	void FontSave(void);
	void bitblt(int X,int Y,int Len,int Hei, char far* Data, char State);
	void bitblt(int x,int y,int len,int hei, char far * addr); //бит-блитинг
	void bitbltTP(int x,int y,int len,int hei, char far * addr); //бит-блитинг с прозрачностью
	void bitblt_part(int x,int y,int x_in, int y_in,int Len, int Hei,
int FactLen,int FactHei, char far* Data, char State);
	void border(int x,int y,int len,int hei, int color);	//рисует рамку
	void Center(int y, char far*);	//выводит надпись по центру
	void clear(int=0);				//очистка экрана
	void CreatePalette();			//создать палитру
	void DecrementPalette();		//уменьшить палитру
	void IncrementPalette();		//увеличить палитру

	void GetColorRegister(unsigned char,char&r,char&g,char&b);//получение цветого регистра
	void graphmode();			//установка граф. режима
	unsigned long InputNumber(unsigned int DigitCounter);	//ввод числа
	void InputString(int len, char far * str,int flag=0);//ввод строки в массив str, длиной len, начиная с координат
	void locate(int x,int y);	//изменение координат курсора
	void locateXY(int x,int y);	//установка курсора с точностью до пиксела
        void line(int x1,int y1,int x2,int y2, char color);    //рисует линию
	void line_right(int x,int y,int len, char color);
	void line_down(int x,int y,int hei, char color);
	void mode(char output_mode);    //установка режима отображения: сразу на экран

	SCREEN& operator << (char*);
	SCREEN& operator << (unsigned long);
	SCREEN& operator << (unsigned);
	SCREEN& operator << (int);
	SCREEN& operator << (char);
	SCREEN& operator << (double);

	SCREEN& operator<< (SCREEN& (*_f)(SCREEN&))
				{ return (*_f)(*this); }


	void pause(int n);
	void putpixel(int x,int y,int color); //вывод точки
	void putBigNumb(long num);		//печать числа
	void putBigNumb(long num,char width);	//печать числа с добавлением нулей
	void putBigSym(unsigned char s,char color,char flag);	//вывод большого символа
	void putBigText(char far * text,char color);
	void putBigText(char far * text,char color,char flag);
	void putBigText(char far * text,char color,char flag, char off); //вывод большого текста

	void putSymbol(unsigned char s);			//печать на экране символа
	void putSymbol(int x,int y, unsigned char s);	//печать символа в заданных координатах
	void putText(char far *);		//печать текста
	void putNumb(unsigned long num);			//печать числа
	void putNumb(unsigned long num,char width);	//печать числа с добавлением нулей
	void rectangle(int x,int y,int len,int hei,int color);
	void rectangle(RECT &rt,int color);
	SCREEN(void);				//конструктор
	~SCREEN(void);				//деструктор
	void setbkcolor(char c){bkcolor=c;};	//установка цвета вывода
	void setcolor(char c){color=c;};	//установка цвета вывода
        void SetColorRegister(unsigned char n,char,char,char);//установка цветового регистра
	void Set320x240Mode();			//установить режим 320*240, 256 цветов
	void textmode();			//установка текстового режима
	void update(void);			//обновление экрана
	void update(RECT);			//обновление экрана
	void wait(void);			//ждет обратного хода луча
};
//================== end of class 'SCREEN' ==============================

inline        SCREEN& endl(SCREEN& ob) { return ob; }

class MANIPMY {
public:
	SCREEN& (*fun) (SCREEN& , int);
	int ag;

public:
	MANIPMY ( SCREEN& (*_f)(SCREEN&, int) , int _a)
	{ fun=_f; ag=_a; }

        friend SCREEN& operator<< (SCREEN& obj, MANIPMY& _f)
        {
		(_f.fun) (obj, _f.ag);
		return obj;
	}
};

SCREEN& _setcolor(SCREEN&ob, int);
SCREEN& _setlocate(SCREEN&ob, int);
MANIPMY& setcolor(int);
MANIPMY& cur(int,int);


extern SCREEN scr;

#endif