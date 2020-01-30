#ifndef __GLASS__H
#define __GLASS__H

struct BOX
{
	unsigned char t;
	unsigned char sym;
};
//класс GLASS - стакан тетриса
class GLASS
{
public:

	unsigned

		Len,Hei;	//ширина и высота стакана (в кубиках)

	BOX * Data;		//указатель на данные(стакан)

	int
		x,y;		//координаты стакана на экране(в пикселах)

	int BoxLen, BoxHei;	//размеры одного кубика

		GLASS();				//конструктор
		~GLASS();				//деструктор

//-----------------------------------------------------------------
	int		Save(char *fname);					//сохранить на диске
	int		Load(char *fname);					//загрузить с диска
	void	Clear();                         	//очистка стакана
	void	Make(unsigned len, unsigned hei);	//сделать стакан
	BOX		Read(int j, int i);					//считать кубик
	void	Show();								//показать стакан
	void	ShowBackground(int i);				//показать задний план
	void	ShowBorder();						//показать рамку
	void	Write(int j, int i, BOX bx);		//записать кубик
};

#endif