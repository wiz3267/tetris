#ifndef __FIGURE_H_
#define __FIGURE_H_

#include "glass.h"

//класс FIGURE  - это набор фигур тетриса
//объект однозначно связан с одним из стаканов
class FIGURE
{
public:

	GLASS *Glass;
	//число фигур
	enum { Counter=7 };

	float
		x,y;		//координаты фигуры

	int
		Color,		//цвет фигуры
		Number,		//номер фигуры
		Aspect		//вид фигуры (0..3)
	;
	void	Show();		//нарисовать фигуру в стакане
	void	Show(int x,int y); //нарисовать фигуру на экране
	void	Show(int color);
	int	Check();	//проверка, можно ли рисовать фигуру
	void RandomSym(char *);

	FIGURE();	//конструктор
};

#endif