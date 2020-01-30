     /////////////////////////////////////////////
    // mouse.cpp    //  (c) 23.03.97 Лапшин Е. //
   /////////////////////////////////////////////

   //реализация класса MOUSE

#include "mouse.h"

// ============== 'Draw' - нарисовать мышь =====================
void MOUSE::Draw()
{
	Read();
	//рисуем курсор
	scr->bitbltTP(x,y, Len,Hei, Cursor);
}

//конструктор
MOUSE::MOUSE(SCREEN *screen)
{
	static char Data[]=
	{
/*		16,16,16,16,16,0,0,0,
		16,15,15,15,16,0,0,0,
		16,15,15,15,16,0,0,0,
		16,15,15,15,15,16,0,0,
		16,16,16,15,15,15,16,0,
		0,0,0,16,15,15,15,16,
		0,0,0,0,16,15,15,16,
		0,0,0,0,0,16,16,0
*/

		15,15,15,15,15,0,0,0,
		15,15,0,0,0,0,0,0,
		15,0,15,0,0,0,0,0,
		15,0,0,15,0,0,0,0,
		15,0,0,0,15,0,0,0,
		0,0,0,0,0,15,0,0,
		0,0,0,0,0,0,15,0,
		0,0,0,0,0,0,0,0

	};

//	asm mov ax,0
//	asm int 0x33

	scr=screen;
	Len=8;
	Hei=8;
	Cursor=Data;
	Read();		//считываем позицию курсора
}

//деструктор
MOUSE::~MOUSE()
{
}

//читаем данные
void MOUSE::Read()
{
	int x1;
	int y1;
	char butt;
	asm mov ax,3
	asm int 0x33
	asm mov x1,cx
	asm mov y1,dx
	asm mov butt,bl
	x=x1>>1,y=y1,Buttons=butt;
}

void MOUSE::set_horz_movement(int min_x, int max_x)
{
	int min=min_x, max=max_x;
        asm {
        	mov ax,7
                mov cx,min
                mov dx,max
                int 0x33
        }

}


void MOUSE::set_vert_movement(int min_y, int max_y)
{
	int min=min_y, max=max_y;
        asm {
        	mov ax,8
                mov cx,min
                mov dx,max
                int 0x33
        }
}

//установить курсор мыши в соотвествии с текущими значениями
void MOUSE::Set()
{
	int _x=x,_y=y;
	asm {
		mov ax,0x04
		mov cx,_x
		mov dx,_y
		int 0x33
	}
}