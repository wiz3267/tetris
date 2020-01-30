#ifndef __SPRITE_H
#define __SPRITE_H

//**************************************************************
//***************** описание класса SPRITE *********************
//**************************************************************

// Процедуры вывода спрайтов подразумевают режим экрана 320*200, 256 color

class SCREEN;	//предварительное 'taltentive' объявление класса


struct FILEHEADER
{
	char Signature[10];
	long Info;		//смещение до структуры SPRITEINFO от начала файла
        long Sprite;		//смещение до спрайта от начала файла
        long FileSize;		//размер файла (для проверки целостности)
};

//благодоря такой организации, можно наращивать, если это необходимо
//эту структуру, если ее расположить в конце файла,
//тогда при считывании недостающие элементы просто не будут читаться

//структура формата образа
struct SPRITEINFO
{
	int Len, Hei;	//размеры спрайта
	int Attribute;	//атрибуты спрайта
};


class SPRITE
{
public:

	//=========== структура 'IMAGE' - образ спрайта=================
	struct IMAGE
	{
		unsigned Len;		//длина
		unsigned Hei;		//ширина
                //если Datacolor==0,//спрайт считается свободным
		char far *Datacolor;	//указатель данных о цвете точек самого спрайта
		int	Attribute;	//свойства спрайта
		int Index;		//идентификатор спрайта
                void Write(int x,int y,unsigned char color);	//запись точки в спрайт
                unsigned char  Read(int x,int y);	//чтение точки в спрайт
	};


	//максимальное число спрайтов, которое можно создать для одного обьекта
	//класса SPRITE
	int SPRITES_MAX;

	//атрибуты спрайта
	enum {
	TRANSPARENT=0x04	//спрайт с прозрачными пикселами
	};


private:
static	int	CountObject;	//число объектов

	IMAGE	*Sprites;	//адрес массива структур типа IMAGE
	char	*DoubleBuffer;	//адрес теневого экрана
public:
	int	Counter;	//счетчик спрайтов
	SCREEN	*scr;		//указатель на объект типа SCREEN

	//конструктору необходимо  передать адрес теневого экрана
	SPRITE(SCREEN far *, int spMax=100);

	//деструктор - уничтожает все созданные спрайты и сам обьект
	//класса SPRITE
	~SPRITE(void);

	//удалить спрайт с индексом n
	int Delete(int n);

	int find(int Index);	//поиск спрайта с индексом Index

        int find_free();	//возвращает номер первого пустого спрайта

	//загрузить спрайт с диска как массив цветных точек
	int Load(char * fname, unsigned long offset, IMAGE );

        //загрузить спрайт с диска(файл имеет определенный формат)
	int LoadSpriteform(char * fname, IMAGE & im);

	//изменяет цвет каждой точки у спрайта i на величину n
	void ModifyColor(int i, char n);

        //создается новый спрайт. Содержание спрайта не определено
        int Create(IMAGE&);

	//если спрайт имеет свойства по умолчанию, можно использовать
	//такой способ загрузки
//	void Load(char far * fname, int len, int hei, int Index);

	//загрузить спрайт с диска, и в дальнейшем обращаться к нему
	// по возвращенному номеру
//	int  Load(char far * fname);

//	//установка образа спрайта
//	void SetImage(IMAGE Image, int Index);

	//получения образа спрайта
	IMAGE far & GetImage(int Index);

//	//установить атрибуты спрайта с индексом Index
//	void SetAttribute(int Attribut, int Index);

	//сдвиг спрайта Index, в сторону F, на Num точек
        enum {
		SCROLL_UP=0x01,
		SCROLL_DOWN=0x02,
		SCROLL_LEFT=0x03,
		SCROLL_RIGHT=0x04
        };

	int Scroll(int Index, int Flags, int Num=1);

	//вывести на экран спрайт с индексом Index
	int Show(int x,int y, int Index,int=0);

	//вывести на экран спрайт с указанием номера в массиве
        int ShowDirect(int x,int y,int Index,int=0);

};

#endif