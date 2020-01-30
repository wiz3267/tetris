#ifndef __WINDOWS_H
#define __WINDOWS_H

#include "rects.h"
#include "debug.h"

typedef signed long LONG;
typedef LONG LRESULT;
typedef unsigned int UINT;
typedef LONG LPARAM;
typedef LONG WPARAM;
typedef UINT HWND;


//********************************************************************
//************************** Коды сообщений **************************
//********************************************************************
enum {

        WM_LBUTTONDOWN,	//нажата левая кнопка мыши во внутренней области окна
        WM_RBUTTONDOWN,//нажата правая кнопка мыши во внутренней области окна

        WM_LBUTTONUP, //отпущена левая кнопка мыши во внутренней области окна
        WM_RBUTTONUP,//отпущена правая кнопка мыши во внутренней области окна
        WM_MOUSEMOVE,//перемещение мыши во внутренней области окна

        WM_COMMAND   //сообщение от кнопки

};

//*********************************************************************
//************ структура MESSAGE - описывает сообщение ****************
//*********************************************************************
struct MESSAGE
{
	UINT Code;
        WPARAM wParam;
        LPARAM lParam;
        MESSAGE(UINT m,WPARAM wp,LPARAM lp) : Code(m), wParam(wp), lParam(lp) {;}
        MESSAGE() : Code(0), wParam(0), lParam(0) {;}
};


//********************************************************************
//**************** класс WINDOW - окно *******************************
//********************************************************************
class WINDOW
{
        friend MOTHERWINDOW;

protected:
        WINDOW *Next;		//указатели на следующее
        WINDOW *Prev;		//и предыдущее окно в списке

        MOTHERWINDOW *Mother;	//указатель на окно-мать

        int x,y,len,hei;	//координаты окна относительно матери
        RECT Rect;		//местоположение окна относительно начала экрана
        char *text;		//текст окна
        int ID_Number;		//уникальные номер окна
	int Status;		//состояние окна
public:
	void RenewRect();
//	WINDOW(RECT, int id, char *str);
	WINDOW(int,int,int,int, int id, char *str);
virtual	~WINDOW()
	{
		if (text) delete text;
		else Debug(" Window::~Window text == 0 ?! ");
	}

	//этим делаем класс абстрактным
virtual void Show()=0;		//показать окно
	//функция окна
	//msg - код сообщения, wParam, lParam - дополнительные параметры
virtual	LRESULT WndProc(MESSAGE&) = 0;
//	void MousePress(int);	//нажали на кнопку мыши
	void Set(int x,int y,int len,int hei, int id, char *txt);
};

//********************************************************************
//****************** класс MOTHERWINDOW материнское окно *************
//********************************************************************
class MOTHERWINDOW
{
        friend BUTTON;
        friend WINDOW;

	RECT Place;		//расположене окна
        int ID_Mother;		//уникальные код окна - матери
        int Status;		//состояние окна - матери

        char Background;	//цвет окна матери (заднего плана для дочерних)

        int ActiveWindow;	//кнопка с рамкой

	WINDOW *First;		//указатели на первое и последнее окно
        WINDOW *Last;		//списка

	int Counter;		//число окон в списке

public:

	LRESULT (*WndProc)(MESSAGE&);	//указатель на функцию окна

        MOTHERWINDOW(int x,int y,int len,int hei, int id);
        MOTHERWINDOW(RECT rt, int id);
	int Add( WINDOW *ob);	//добавление окна в список
        void moveto(int dx, int dy);	//переместить окно относительно его
        				//координат
        void move(int x,int y);		//установить новые координаты окна
        void ShowAll();			//показать все окна в списке
        void SendMessages();		//послать дочерним окнам сообщения
        void SetBackground(char);
        int Del(int ID_Number);		//удалить определенное дочернее окно
	void Main(MESSAGE&);
};

//**********************************************************************
//**************************** класс кнопка ****************************
//**********************************************************************
class BUTTON : public WINDOW
{
	int TextColor;	 //цвет текста
        int BorderColor; //цвет рамки вокруг кнопки

public:
//возможное состояние окна
        enum
	{
		PRESS   = 0x0001,	//кнопка нажата
		NOPRESS = 0x0002,	//кнопка ненажата
                BORDER  = 0x0004,	//вокруг кнопки надо рисовать рамку
	};

	BUTTON(int x,int y,int len,int hei, int id, char *txt);
	BUTTON(int x,int y,int id, char *txt);
	LRESULT WndProc(MESSAGE&);
        void Show();
};

#endif