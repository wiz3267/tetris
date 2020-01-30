////////////////////////////////////////
// Copyright (C) Лапшин Евгений, 1997 //
////////////////////////////////////////


#include <mem.h>
#include <string.h>

#include "windows.h"
#include "debug.h"
#include "screen.h"
#include "mouse.h"
#include "keyboard.h"



//**************************************************************************
//************ класс MOTHERWINDOW - материнское окно ***********************
//**************************************************************************

//сюда передаются сообщения, если нет процедуры
LRESULT default_windows_procedure(MESSAGE&)
{
	return 0;
}

//конструктор материнского окна
MOTHERWINDOW::MOTHERWINDOW(int x,int y,int len,int hei, int id) :
Counter(0), First(0), Last(0), Status(0), ID_Mother(id), Place(x,y, x+len-1,
y+hei-1), ActiveWindow(0), WndProc(&default_windows_procedure)
{
}

// -------- Add - добавить окно в список -----------
// В функцию передается указатель на объект, созданный только и только
// оперецией new.
// Внимание ! Это объект не надо удалять операцией delete. Это сделает
// деструктор класса MOTHERWINDOW ! Помните об этом
int MOTHERWINDOW::Add( WINDOW *NewWin)
{
	//если память не выдана
	if (!NewWin)
	{
		Debug("MOTHERWINDOW::Add - what is NewWin !?.");
		return 0;
	}

	//запоминаем ссылку на мать
	NewWin->Mother=this;

       //обновляем структуру Rect
       NewWin -> RenewRect();

	//если это первое окно, включаемое в список
	if (!Counter)
	{
		NewWin->Prev=0;
		NewWin->Next=0;
		First=NewWin;
		Last=NewWin;
	}
	//иначе окна уже есть
	else
	{
		First->Prev=NewWin;
		NewWin->Next=First;
		First=NewWin;
	}

	Counter++;
	return 1;
}

void MOTHERWINDOW::SetBackground(char r)
{
	Background=r;
}

//показать все окна
void MOTHERWINDOW::ShowAll()
{
        scr.rectangle(Place, Background);

	WINDOW * ob = First;
        for(int i=0; i<Counter; i++)
        {
		ob -> Show();

                ob = ob->Next;
        }

}


int Shoot(int x, int y, RECT &rt)
{
	if (x>=rt.Left && x<=rt.Right && y>=rt.Top && y<=rt.Bottom) return 1;
        else return 0;
}


//главная функция обрабатывает сообщения от мыши и от клавиатуры
//и распределяет дочерним окнам
void MOTHERWINDOW::Main(MESSAGE &msg)
{
  int x=msg.wParam;
  int y=msg.lParam;

  WINDOW *win=Last;

  switch(msg.Code)
  {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:

		for(int i=Counter-1; i>=0; i--)
                {
                	if (Shoot(x,y, win->Rect))
	                {
        	        	win->WndProc(msg);
                                break;
	                }
                        win=win->Prev;
                }
      break;

  }
}



extern MOUSE Mouse;

//посылаем сообщения окнам
void MOTHERWINDOW::SendMessages()
{

        //если нет кнопок, вернуться
	if (!Counter) return;

        if (Keyb[KEY_TAB] == 1)
	{
                Keyb.Set(KEY_TAB,3);
                ActiveWindow++;
		if (ActiveWindow >= Counter) ActiveWindow=0;

	}

	//с конца
        WINDOW *win=Last;
        int x=Mouse.x, y=Mouse.y;

        MESSAGE msg;


	//отпускаем кнопку
	if (
		//если отпущена кнопка мыши
		(~Mouse.Buttons & MOUSE::BUTTON_LEFT )

			// и
//			&&

		//отпущена клавиша <ENTER>
  //		( !Keyb[KEY_ENTER] )
	   )

	{
		//будем пытаться отпустить кнопку

		//поиск кнопки под курсором мыши
		for(int i=Counter-1; i>=0; i--)
		{
                	if (Shoot(x,y, win->Rect))
			{
				//если кнопка нажата, ее отпускаем, иначе
				//нет. Отпущенную кнопку отпустить еще раз
				//нельзя
				if (win->Status == BUTTON::PRESS)
				{
					msg.Code=WM_LBUTTONUP;
					win->WndProc(msg);
				}
				break;

			}
			win=win->Prev;
                }
         }


	/*
	win=Last;

	//делаем все кнопки ненажатыми
	for(int i=Counter-1; i>=0; i--)
	{
		//msg.Code=WM_LBUTTONUP;
		//win->WndProc(msg);
		win->Status=2;

		//if ( i==ActiveWindow) win->Status |= WINDOW::BORDER;
		//else win->Status &= ~WINDOW::BORDER;

		win=win->Prev;
	} */

	win=Last;

	//если
	if
	  (
			//нажата левая кнопка мышки
			(Mouse.Buttons & MOUSE::BUTTON_LEFT)

				    //или
    //				    ||

			//нажата клавиша <ENTER>
      //			( Keyb[KEY_ENTER] )

	  )

	{
                //сканируем до первой нажатой кнопки
		for(int i=Counter-1; i>=0; i--)
		{
                	if (Shoot(x,y, win->Rect))
	                {
                                msg.Code=WM_LBUTTONDOWN;
        	        	win->WndProc(msg);
				break;
	                }
                        win=win->Prev;
                }
         }

}

//**********************************************************
//************ класс WINDOW - окно ***********************
//**********************************************************


/*WINDOW::WINDOW(RECT rt, int id, char *str)
{
        x=rt.Left;
        y=rt.Top;
        len=rt.Right-x+1;
        hei=rt.Bottom-y+1;

        Rect = rt;
	ID_Number=id;
	State=NOPRESS;

        int k=strlen(str)+1;
	text=new char[k];
        _fmemcpy(text, str, k);
}*/

/*
void WINDOW::Set(int x1,int y1,int len1,int hei1, int id, char *str)
{
	x=x1;
	y=y1;
	len=len1;
	hei=hei1;

	ID_Number=id;

	int k=strlen(str)+1;
	text=new char[k];
	_fmemcpy(text, str, k);
}
*/

WINDOW::WINDOW(int x1,int y1,int len1,int hei1, int id, char *str)
{
	x=x1;
	y=y1;
	len=len1;
	hei=hei1;

	ID_Number=id;

	int k=strlen(str)+1;
	text=new char[k];
	_fmemcpy(text, str, k);
}


//обновить структуру Rect
void WINDOW::RenewRect()
{
	int dx=Mother->Place.Left;
        int dy=Mother->Place.Top;
        Rect.Set(x+dx, y+dy, x+dx+len-1, y+dy+hei-1);
}

//**********************************************************
//************ класс Button - кнопка ***********************
//**********************************************************

/*
BUTTON::BUTTON(RECT rt, int id, char *str) : WINDOW(rt, id, str)
{
	TextColor=0;
}*/

BUTTON::BUTTON(int x1,int y1,int len1,int hei1, int id, char *str)
: WINDOW(x1,y1,len1,hei1,id, str)
{
	TextColor=0;
        BorderColor=4;

        Status = NOPRESS;
}

//этот конструктор сам определяет высоту и длину кнопки
BUTTON::BUTTON(int x1,int y1,int id, char *str)
: WINDOW(x1,y1,strlen(str)*8+13, 15, id, str)
{
	TextColor=0;
        BorderColor=4;

        Status = NOPRESS;
}


void BUTTON::Show()
{
        int x1=x+Mother->Place.Left;
        int y1=y+Mother->Place.Top;

        //если надо нарисовать рамку вокруг кнопки
        //рисуем рамку вокруг кнопки
        if (Status & BORDER)
        {
	        scr.line_down(x1,y1, hei, BorderColor);
        	scr.line_down(x1+len-1,y1, hei, BorderColor);

	        scr.line_right(x1+1,y1, len-2, BorderColor);
        	scr.line_right(x1+1,y1+hei-1, len-2, BorderColor);
        }

        x1++; y1++; int len1=len-2, hei1=hei-2;

        if (
		Status & PRESS
				&&
		Status & NOPRESS
	   )
        {
        	fatal("Button PRESS and NOPRESS!");
        }

        //рисуем саму кнопку
        //если кнопка нажата
        if (Status & NOPRESS)
        {
                //окантовка кнопки
                scr.line_down(x1,y1, hei1-1, 31);
                scr.line_right(x1+1,y1, len1-2,31);

                scr.line_down(x1+len1-1, y1, hei1, 21);
                scr.line_right(x1,y1+hei1-1, len1-1,21);

		scr.rectangle( x1+1, y1+1, len1-2,hei1-2, 25);

	        //рисуем надпись на кнопке
                int m=strlen(text)*8;
	        scr.locateXY( x1+ (len1-m)/2, y1+(hei1-8)/2 );
        	scr.setcolor(TextColor);
	        scr<<text;
        }
        //иначе рисуем кнопку ненажатой
        if (Status & PRESS)
        {
                //окантовка кнопки
                scr.line_down(x1,y1, hei1-1, 0);
                scr.line_right(x1+1,y1, len1-2,0);

                //scr.line_down(x1+len-1, y1, hei, 0);
                //scr.line_right(x1,y1+hei-1, len-1,0);

		scr.rectangle( x1+1, y1+1, len1-1,hei1-1, 25);

	        //рисуем надпись на кнопке
                int m=strlen(text)*8;
	        scr.locateXY( x1 + (len-m)/2 + 1, y1+(hei-8)/2 + 1 );
        	scr.setcolor(TextColor);
	        scr<<text;
        }

}

LRESULT BUTTON::WndProc(MESSAGE& msg)
{
	switch(msg.Code)
        {
          //если нажали на левую кнопку мышки
          case WM_LBUTTONDOWN:
          {
                Status=PRESS;
          	break;
          }

          //если отпустили левую кнопку мышки
          case WM_LBUTTONUP:
          {
                Status=NOPRESS;
                MESSAGE msg;
                msg.Code = WM_COMMAND;
                msg.wParam = ID_Number;
                Mother->WndProc(msg);
          	break;
          }
        }
}

