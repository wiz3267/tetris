////////////////////////////////////////
// Copyright (C) Лапшин Евгений, 1997 //
////////////////////////////////////////


//*****************************************************************
//********** SCREEN.CPP - функции-члены класса SCREEN *************
//*****************************************************************
#define TRUE 1
#define FALSE 0

#include <mem.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>

#include "screen.h"
#include "debug.h"
#include "rects.h"
#include "keyboard.h"
#include "timer.h"

#include "string.h"

#include <fstream.h>


//создаем предопределенный объект
SCREEN  scr;

//инициализируем статические элементы

  int    SCREEN::CountObjects=0;  //число созданных объектов типа SCREEN
  char far * SCREEN::DoubleBuffer=0;  //указатель на дублирующий буффер
  char     SCREEN::constructor_run=FALSE; //был ли запущен конструктор
  char far * SCREEN::VideoMemory=(char far*) 0xA0000000L; //адрес физического экрана

FONT Font;



unsigned char Font8x8[2048];

//гасит плавно экран
void SCREEN::DecrementPalette()
{
  for(int i=0; i<256; i++)
  {
	char r,g,b;
	scr.GetColorRegister(i, r,g,b);

	if (r) r--;
	if (g) g--;
	if (b) b--;

	scr.SetColorRegister(i, r,g,b);
  }

}

SCREEN Screen;

void SCREEN::FontSave()
{
	ofstream ofl;
	ofl.open("font.fnt", ios::binary);
	ofl.write(Font8x8,2048);
	ofl.close();
}


//================== конструктор ===============================
SCREEN::SCREEN(void)
{
  StepX=8;
  //загружаем шрифт
  for(int i=0; i<2048; i++) Font8x8[i]=i;
  ifstream ifl;
  ifl.open("font.fnt", ios::binary);
  ifl.read(Font8x8,2048);
  ifl.close();

  constructor_run=TRUE;
  CountObjects++;

  //инициализируем переменные
  x=0;  //координаты пера - левый верхний угол экрана
  y=0;

  color=15; //цвет - ярко белый
  bkcolor=0;  //фон прозрачен

  //если это самый первый объект, то надо переключиться в графический
  //режим (320*200 при 256 цветах, палитра 262144 цвета)
  if (CountObjects==1)
  {
	graphmode();
  }

  //память под DoubleBuffer надо выделить только для первого объекта
  if (CountObjects==1)
  {
    DoubleBuffer=new char[320*200];
    clear();         //очистка экрана
  }

  //если под дублирующий буффер память выделить не удалось
  if (!DoubleBuffer)
  {
    //в этом случае для вывода картинки используем экран
    //???
    DoubleBuffer=VideoMemory;
    Debug("no RAM for DoubleBuffer. Use screen");
  }

}
//========================= end of 'SCREEN' ============================

//===================деструктор================================
SCREEN::~SCREEN(void)
{
  #if DEBUGMESSAGES
    Debug("Run SCREEN destructor");
  #endif

  //если конструктор был запущен
  if (constructor_run)
  {
    //уменьшаем число объектов
    CountObjects--;

    //если удаляется последний объект,надо освободить память
    if (CountObjects==0)
    {
      if (DoubleBuffer!=VideoMemory)
      {
        delete DoubleBuffer;
      }
      textmode();   //переключаемся в текстовый режим
	}
  }
}
//========================= end of '~SCREEN' ============================


//======== 'bitblt_part' - выполняет копирование части спрайта на экран=========
//x,y - координаты вывода
//x_in,y_in - координаты внутри спрайта
//Len,Hei - длина спрайта
//FactLen, FactHei - размеры выводимого блока спрайта
// Data - адрес данных
// State - как выводить спрайт
void SCREEN::bitblt_part(int x,int y,int x_in, int y_in,int Len, int Hei,
int FactLen,int FactHei, char far* Data, char State)
{
  //проверка параметров на допустимость
  if (x<0 || y<0 || x>320-FactLen || y>200-FactHei)
  {
	Debug("bitblt_part x,y is bad");
	return;
  }

  if (x_in<0 || y_in<0)
  {
	Debug("bitblt_part x_in,y_in <0");
	return;
  }

  if (x_in>=Len || y_in>=Hei)
  {
	Debug("bitblt_part x_in>=Len or y_in>=Hei");
	return;
  }

  if (FactLen>Len || FactHei>Hei)
  {
	Debug("bitblt_part FactLen>Len or FactHei>Hei");
	return;
  }

  if (FactLen<0 || FactHei<0)
  {
	Debug("bitblt_part FactLen<0 || FactHei<0");
	return;
  }

  if (State & FLIP_HORZ){
	if (x_in==0) x_in=Len-FactLen;
	else x_in=0;
  }

  int ONELINE=320;  //длина одной линии

  if (State & FLIP_VERT)
  {
//                State ^= FLIP_VERT;
	ONELINE=-320;
	if (y_in==0) y_in=Hei-FactHei;
	else y_in=0;

  }



  char far *Sour=Data;
  char far *Dest=DoubleBuffer;

  Dest+=y*320+x;  //вычисляем адреса назначения
  Sour+=y_in*Len+x_in;  //вычисляем адрес внутри спрайта

  switch (State)
  {
	case 0:

  asm {
	push ds
	push es

	lds si,Sour
	les di,Dest
  }
  loop1:
  asm {
	cld
	mov cx,FactLen
	rep movsb

	sub di,FactLen
	sub si,FactLen

	add di,ONELINE
	add si,Len

	dec FactHei
	jnz loop1

	pop es
	pop ds
  }
	break;

	//есть прозрачные пикселы
	case TRANSP:
	asm {
	  push ds
	  push es

	  lds si,Sour
      les di,Dest

      mov cx,FactLen
      cld
    }
      l2:
    asm {
      lodsb
      or al,al
      jz l3
      mov es:[di],al
    }

      l3:
      asm { inc di
	  dec cx
      jnz l2

      sub si,FactLen
      add si,Len

      sub di,FactLen
      add di,ONELINE

      mov cx,FactLen

      dec FactHei
      jnz l2

      pop es
      pop ds
    }
      break;

    //спрайт надо повернуть по горизонтали
	case FLIP_HORZ | TRANSP:
    asm {
      push ds
      push es

      lds si,Sour
      les di,Dest
      add di,FactLen
      dec di

      mov cx,FactLen

      mov dx,FactHei

      cld }
      l4:
    asm {
      lodsb

      or al,al
	  jz lopsi
      mov es:[di],al }
    lopsi:
    asm {
      dec di

      dec cx
      jnz l4

      sub si,FactLen
      add si,Len

      add di,ONELINE
      add di,FactLen


      mov cx,FactLen

      dec dx
      jnz l4

      pop es
      pop ds
    }
      break;

    //спрайт надо повернуть по горизонтали
    case FLIP_HORZ:
    asm {
      push ds
      push es

      lds si,Sour
      les di,Dest
      add di,FactLen
      dec di

      mov cx,FactLen

      mov dx,FactHei

      cld }
      l6:
    asm {
      lodsb

      mov es:[di],al
      dec di

      dec cx
      jnz l6

      sub si,FactLen
      add si,Len

      add di,ONELINE
      add di,FactLen


      mov cx,FactLen

      dec dx
      jnz l6

      pop es
      pop ds
    }
      break;



  } //конец switch
}
//========================= end of 'bitblt_part' ============================

//================ bitblt общая процедура вывода спрайта ====================
void SCREEN::bitblt(int xc,int yc,int Len,int Hei, char far* Data, char State)
{

  int ONELINE=320;

  //если спрайт полностью вышел за пределы экрана
  if ( xc<=-Len || yc<=-Hei || xc>=320 || yc>=200)
  {
//    Debug("'bitblt' - x or y out of side");
    return;
  }

  //вычисление адреса вывода
  char far * vb=DoubleBuffer+xc+yc*320;

  //если спрайт полностью помещается на экране
  if ( xc>=0 && yc>=0 && xc<=320-Len && yc<=200-Hei)
  {

    if (State & FLIP_VERT) {
    vb+=(Hei-1)*320; ONELINE=-320;
    State ^= FLIP_VERT;
    }

	   switch (State)
       {
    //вывод обычного спрайта.Нет прозрачности
    case 0:
    asm {
      push ds
      push es
      lds si,Data
      les di,vb
      mov dx,Hei
      cld
      mov ax,ONELINE
      }
      l1:
    asm {
      mov cx,Len
      mov bx,cx
      shr cx,2

      and bx,3

      or bx,bx
      jz l1_cont
      movsb
      dec bx

      or bx,bx
      jz l1_cont
      movsb
      dec bx

      or dx,dx
      jz l1_cont
      movsb
      dec bx

    }


    l1_cont:
	asm {
      db 0x66
      rep movsw

      add di,ax
      sub di,Len
      dec dx
      jnz l1
      pop es
      pop ds
    }
      break;

    //Вывод обычного спрайт есть прозрачные пикселы
    case TRANSP:
    asm {
      push ds
      push es

      lds si,Data
	  les di,vb

      mov cx,Len
      mov dx,Hei
      cld }
      l2:
    asm {
      lodsb
      or al,al
      jz l3
      mov es:[di],al
    }

      l3:
      asm { inc di
      dec cx
      jnz l2:

      add di,ONELINE
      mov cx,Len
	  sub di,cx
      dec dx
      jnz l2
      pop es
      pop ds
    }
      break;


    //спрайт надо перевернуть по горизонтали.Нет прозрачности
    case FLIP_HORZ:
    asm {
      push ds
      push es

      lds si,Data
      add si,Len
      sub si,2

      les di,vb

      mov cx,Len
      shr cx,1

      mov dx,Hei

      std }
      l4:
    asm {
      lodsw

      xchg al,ah

      mov es:[di],ax
      add di,2

      dec cx
      jnz l4

      mov cx,Len
	  add si,cx
      add si,cx

      add di,ONELINE
      sub di,cx

      shr cx,1

      dec dx
      jnz l4

      pop es
      pop ds

    }
      break;

    //надо повернуть по горизонтали.Eсть прозрачные пикселы
    case FLIP_HORZ|TRANSP:
    asm {
	  push ds
      push es

      lds si,Data
      add si,Len
      sub si,2

      les di,vb

      mov cx,Len
      shr cx,1

      mov dx,Hei

      std }
      l8:
    asm {
      lodsw
      or ah,ah
      jz l6
	  mov es:[di],ah }
    l6:
    asm { inc di
      or al,al
      jz l7
      mov es:[di],al }
    l7:
    asm { inc di

      dec cx
      jnz l8

      mov cx,Len
      add si,cx
      add si,cx

      add di,ONELINE
      sub di,cx

      shr cx,1

      dec dx
      jnz l8

      pop es
      pop ds

    }
    break;
       }//конец switch
  } //конец if

  //иначе спрайт помещается на экране не полностью - необходимо выполнить
  //отсечение
  else
  {
    //если спрайт по оси y поместился полностью

    if (yc>=0 && yc<=200-Hei)
    {
	  if (xc<0)
        bitblt_part(0,yc,
          0-xc, 0,  //координаты внутри спрайта
          Len,Hei,  //настоящая длина
          Len+xc, Hei,  //факт. длина
          Data, State);
      else
        bitblt_part(xc,yc,
          0, 0, //координаты внутри спрайта
          Len,Hei,  //настоящая длина
          320-xc, Hei,  //факт. длина
          Data, State);

      return;
    }//конец if


                //если спрайт по оси Ox поместился полностью
          if (xc>=0 && xc<=320-Len)
                {

                        if (yc<0)
                          bitblt_part(xc,0,
          0, 0-yc,  //координаты внутри спрайта
          Len,Hei,  //настоящая длина
          Len, Hei+yc,  //факт. длина
          Data, State);
                        else
                          bitblt_part(xc,yc,
          0, 0, //координаты внутри спрайта
          Len,Hei,  //настоящая длина
          Len, 200-yc,  //факт. длина
          Data, State);

                  return;
                }



                //если спрайт в левом верхнем углу
				if (xc<0 && yc<0)
                {
                        bitblt_part(0,0,
        -xc, -yc, //координаты внутри спрайта
        Len,Hei,  //настоящая длина
        Len+xc, Hei+yc, //факт. длина
        Data, State);

                        return;
                }

                //если спрайт в левом нижнем углу
                if (xc<0 && yc>200-Hei)
                {
                        bitblt_part(0,yc,
        -xc, 0, //координаты внутри спрайта
        Len,Hei,  //настоящая длина
        Len+xc, 200-yc, //факт. длина
        Data, State);

						return;
                }

                //если спрайт в правом нижнем углу
                if (xc>320-Len && yc>200-Hei)
                {
                        bitblt_part(xc,yc,  //коорд. вывода на экран
        0, 0,   //координаты внутри спрайта
        Len,Hei,  //настоящая длина
        320-xc, 200-yc, //факт. длина
        Data, State);

                        return;

                }


                //если спрайт в правом верхнем углу
                if (xc>320-Len && yc<0)
                {
						bitblt_part(xc,0, //коорд. вывода на экран
        0, -yc,   //координаты внутри спрайта
        Len,Hei,  //настоящая длина
        320-xc, Hei+yc, //факт. длина
        Data, State);
                        return;

                }

        }//конец else
}
//========================= end of 'bitblt' ============================


//=============== 'bitblt' - вывод спрайта на экран ===============
//без учета прозрачности
void SCREEN::bitblt(int x,int y,int len,int hei,char far * source)
{
  if ( y<0 || x<0 || x>320-len || y>200-hei)
  {
//    Debug("'bitblt' - x or y out of side");
    return;
  }
  len/=4; //делим на 4
  char far * vb=DoubleBuffer;
  unsigned int addr=x+y*320;
  asm  {
    push ds
    lds si,source
    les di,vb
    add di,addr
    cld
    mov dx,len
    mov bx,320
    mov ax,hei
       }
loops:
  asm {

    mov cx,dx
	db 0x66
    rep movsw // movsd
    add di,bx
    sub di,dx
    sub di,dx
    sub di,dx
    sub di,dx
    dec ax
    jnz loops
    pop ds
     }

}
//========================= end of 'bitblt' ============================


//=========== 'bitbltTP' - вывод спрайт с учетом прозрачности ==========
// ( TP - transparent )
void SCREEN::bitbltTP(int x,int y,int len,int hei,char far * source)
{
  if ( y<0 || x<0 || x>320-len || y>200-hei)
  {
//    Debug("'bitbltTP' - x or y out of side");
    return;
  }

  char far * vb=DoubleBuffer;
  unsigned int addr=x+y*320;
  asm  {
    push ds
    lds si,source
    les di,vb
    add di,addr
    mov cx,len
    shr cx,1
    cld
       }
loops:
      asm push cx
cnt1:
	 asm {
      lodsw
      or al,al
      jz cnt
      mov es:[di],al
        }
cnt:
        asm or ah,ah
        asm jz cnt2
        asm mov es:[di+1],ah
cnt2:
        asm {
      add di,2
      loop cnt1
    pop cx

    add di,320
    sub di,len
    dec hei
    jnz loops

    pop ds
    }
}
//===================== end of 'bitbltTP' =================================

void SCREEN::border(int x,int y,int len,int hei, int color) //рисует рамку
{
  line_down(x,y,hei, color);
  line_down(x+len-1,y,hei,color);
  line_right(x,y,len,color);
  line_right(x,y+hei-1,len,color);
}

//выводит надпись по центру
void SCREEN::Center(int y, char far*str)
{
  scr.locateXY( (320-strlen(str)*8)/2, y);
  scr<<str;
}

//================= 'clear' - очистка экрана ============================
void SCREEN::clear(int c)
{
  c++;
  char far * d=DoubleBuffer;
  asm {
    les di,d
    cld
    db 0x66 //mov eax,0x00000000 (for >= 80386)
    mov ax,0
    dw 0
    mov cx,320*200/4
    db 0x66 // rep stosd (4 байта) (for >= 80386)
    rep stosw
        }
}

//========================== end of 'clear' =================================

//============ 'GetColorRegister' - получение цветого регистра ==============
void SCREEN::GetColorRegister(unsigned char n,char&r,char&g,char&b)
{
  char R,G,B;
        asm {
          mov al,n
                mov dx,0x3c7
                out dx,al

                mov dx,0x3c9
                in al,dx
                mov R,al

                in al,dx
                mov G,al

                in al,dx
                mov B,al
        }
		r=R; g=G; b=B;

}
//====================== end of 'GetColorRegister' ==========================


//=============== graphmode - переключение в графический режим ==============
void SCREEN::graphmode()
{
  asm mov ax,0x13
  asm int 0x10;   //переключаемся в графический режим

  //создаем палитру
  int t=144;
  char r,g,b;
  for (int i=1;i<=7;i++)
  {
    for (int k=0;k<=15;k++,t++)
    {
	  if (i & 1)  b=18+3*k;
      else          b=0;
	  if (i & 2)  g=18+3*k;
      else    g=0;
      if (i & 4)  r=18+3*k;
      else    r=0;

      SetColorRegister(t, r,g,b);

    }            //конец for k
  }        //конец for i

}
//====================== end of 'graphmode' =============================


//================= 'InputNumber' - ввод числа =============================
// DigitCounter>=1 - число цифр
unsigned long SCREEN::InputNumber(unsigned int DigitCounter)
{
  if (!DigitCounter || DigitCounter>10)
  {
	Debug("SCREEN::InputNumber - bad DigitCounter");
    return 0;
  }

  char *str=new char[DigitCounter+1];
  if (!str)
  {
    Debug("SCREEN::InputNumber - out of memory");
    return 0;
  }

  //вводим строку
  InputString(DigitCounter+1, str, ONLY_DIGIT);

  //переводим её в число

  unsigned long number=atol(str);

  delete str;

  return number;

}

//================= 'InputString' - ввод строки =============================
//в массив str, длиной len, начиная с текущих координат
// если flag==ONLY_DIGIT - тогда вводятся только цифры
void SCREEN::InputString(int len, char far * str, int flag)
{
  Keyb.StandartInput();

  int x=scr.x;
  int y=scr.y;

  int i=0;  // i - указывает на элемент массива, в который должен
      // быть осуществлен ввод
  while (1)
  {
	unsigned char s;
    if (kbhit())
	  s=getch();
    else
      s=1;

    if ( (Timer.GetTotalCounter() % 5) <2)
    {
      scr.rectangle(x+i*8,y+6,8,2,scr.color);
      scr.update();
    }
    else
    {
      scr.rectangle( x+i*8,y+6,8,2,scr.bkcolor);
      scr.update();
    }

    if (!s)
    {
      getch();
      continue;
    }

    //интер - ввели
    if ( s==13 )
    {
      str[i]=0;
      //восстанавливаем супер-ввод
      Keyb.SuperInput();

      //делаем клавишу <ENTER> ненажатой
      Keyb.Set(Keyb[KEY_ENTER], 0);

      break;
    }

    if ( s==8 && i)   //забой
    {
      //удаляем курсор
      scr.rectangle( x+i*8,y+6,8,2,scr.bkcolor);

      str[i]=0;
	  i--;
      locateXY(x+i*8,y);
      putSymbol(' ');
      update();
    }

    int yes=0;
    if (flag & ONLY_DIGIT)
    {
      if (s>='0' && s<='9') yes=1;
    }
    else
    {
      if (s>=' ') yes=1;
    }

    if ( yes && i<(len-1) )
    {
      //удаляем курсор
      scr.rectangle( x+i*8,y+6,8,2,scr.bkcolor);

      locateXY(x+i*8,y);
      putSymbol(s);
      update();
      str[i]=s;
      i++;
    }
  }
}
//========================= end of 'InputString' ============================


//================= 'locate'-установка позиции курсора ======================
void SCREEN::locate(int x1,int y1)
{
  x=x1*8;
  y=y1*8;
}
//========================== end of 'locate'  ===============================


//================= 'locateXY'-установка позиции курсора ======================
void SCREEN::locateXY(int x1,int y1)
{
 x=x1;
 y=y1;
}
//==================== end of 'locateXY' ===================================

extern void swap(int&,int&);

//==================== 'line'-рисование прямой линии ========================
void SCREEN::line(int x1,int y1,int x2,int y2, char color)
{
  //* * для рисования линии будем использовать алгоритм Брезенхема * *
        int i, dx, dy, nump,
        d, dinc1, dinc2,
        x, xinc1, xinc2,
        y, yinc1, yinc2;

		dx = abs(x2-x1);
		dy = abs(y2-y1);

        if (dx >= dy)
        {
          nump=dx+1;
                d=2*dy-dx;
                dinc1=dy<<1;
                dinc2=(dy-dx)<<1;
                xinc1=1;
                xinc2=1;
                yinc1=0;
                yinc2=1;
        }
        else
        {
          nump=dy+1;
                d=2*dx-dy;
                dinc1=dx<<1;
                dinc2=(dx-dy)<<1;
				xinc1=0;
				xinc2=1;
                yinc1=1;
                yinc2=1;
        }

        if (x1>x2)
        {
          xinc1*=-1;
                xinc2*=-1;
        }

        if (y1>y2)
        {
          yinc1*=-1;
                yinc2*=-1;
        }

        x=x1;
        y=y1;

		for(i=1; i<=nump; i++)
        {
          putpixel(x,y,color);
                if (d<0)
                {
                  d+=dinc1;
                        x+=xinc1;
                        y+=yinc1;
                }
                else
                {
                  d+=dinc2;
                        x+=xinc2;
                        y+=yinc2;
                }

        }


}
//====================== end of 'line ' ======================================

void SCREEN::line_down(int x,int y,int hei, char clr)
{
  for(int i=y; i<y+hei; i++)
        {
          putpixel(x,i,clr);
        }
}

void SCREEN::line_right(int x,int y,int len, char clr)
{
  for(int i=x; i<x+len; i++)
  {
    putpixel(i,y,clr);
        }
}


//=========================================================================
SCREEN& SCREEN::operator<< (char *string)
{
  putText(string);
		return *this;
}
//=========================================================================


//=========================================================================
SCREEN& SCREEN::operator<< (unsigned long n)
{
  putNumb(n);
		return *this;
}
//=========================================================================

//=========================================================================
SCREEN& SCREEN::operator<< (double n)
{
  char buf[64];
  sprintf(buf,"%.2f",n);

  //putNumb(n);
  putText(buf);
  return *this;
}
//=========================================================================


//=========================================================================
SCREEN& SCREEN::operator<< (unsigned n)
{
  putNumb(n);
		return *this;
}
//=========================================================================



//=========================================================================
SCREEN& SCREEN::operator<< (int n)
{
  putNumb(n);
		return *this;
}
//=========================================================================


//=========================================================================
SCREEN& SCREEN::operator<< (char sym)
{
  putSymbol(sym);
        return *this;
}
//=========================================================================


//=========== 'pause' - выдержка паузы ========================
void SCREEN::pause(int n)
{
  for (;n>=0; n--)
  {
          wait();
  }
}
//=========================================================================


//========== 'putBigSym' - выводит большой символ ========================
int BigSymScale=4;
void SCREEN::putBigSym(unsigned char s,char color,char flag)
{
  //char far * rom_ascii = (char far *) 0xF000FA6EL;
  char far * font = (char far *) Font8x8;
  unsigned int addr=s*8;
  for (int i=0;i<8;i++)
	{
	  s=font[addr];
//      s=1+0+4+0+16+0+64+128;
	  unsigned char mask=0x80;
	  for (int f=0;f<8;f++)
	  {
	   if (flag & RANDOM_COLOR)
	   {
		setcolor(8+rand() % 7);
	   }
	   else if (flag & ONE_COLOR)
	   {
		setcolor(color);
	   }

	   if ( (s & mask))
	   {
		rectangle(x+f*BigSymScale,y+i*BigSymScale,BigSymScale,BigSymScale,color);
	   }
	   mask>>=1;
	}
	  addr++;
	}

	x+=BigSymScale*StepX;

}
//========================================================================



// ============== putBigText - вывод текста ==========================
void SCREEN::putBigText(char far * text,char color)
{
  int i=0;
  while (text[i]!=0) {
     putBigSym(text[i],color,ONE_COLOR);
     i++;
  }
}
// =====================================================================


// ============== putBigText - вывод текста ==========================
void SCREEN::putBigText(char far * text,char color,char flag)
{
  int i=0;
  while (text[i]!=0) {
     putBigSym(text[i],color,flag);
     i++;
  }
}
// =====================================================================


// ============== putBigSymText - вывод большого текста ==================
void SCREEN::putBigText(char far * text,char color,char flag, char off)
{
  int i=0;
  while (text[i]!=0) {
     putBigSym(text[i],color,flag);
     y+=off*64;
     i++;
  }
}
// =====================================================================


//=============== 'putpixel' вывод пиксела ========================
void SCREEN::putpixel(int x,int y,int color)
{
        if (x<0 || x>319 || y<0 || y>199) return;
  DoubleBuffer[x+y*320]=color;
}
//================ end of 'putpixel'==============================


//================= 'putSymbol' - печать символа ===================
// x,y - пиксельные координаты
// s - код символа
void SCREEN::putSymbol(int x,int y,unsigned char s)
{
//  Font.Show(x,y,s, (color % 7)*16  );
  //если за пределами экрана
  if (x<0 || x>312 || y<0 || y>192)
  {
	return;
  }

  char far *dest=(char far *) (DoubleBuffer+y*320+x);
  char far *sour=Font8x8;

/*  if (s>=128)
  {
	s-=128;
	asm {

	  mov ah,0x11
	  mov al,0x30
	  mov bh,0
	  push bp
	  int 0x10
	  mov ax,bp
	  pop bp

	  mov word ptr sour, ax
	  mov word ptr sour+2, es
	}
  }
  else
  {
	sour=(char far *) 0xF000FA6E;
  }*/

  sour+=s*8;
  unsigned char color=this->color;
  unsigned char bkcolor=this->bkcolor;

  unsigned int step_x_int=StepX;
 // unsigned char step_x_char=StepX;
  asm {
	push ds
	push es

	lds si,sour
	les di,dest
	mov dx,8
	mov cx,step_x_int
	cld
  }

  l3:
  asm lodsb

  l2:
  asm test al,0x80
  asm jz l1
  asm mov ah,color
  asm mov es:[di],ah
  asm jmp l5

  //рисовать фон
  l1:
  asm mov ah,bkcolor
  asm or ah,ah
  asm jz l5
  asm mov es:[di],ah
  l5:
  asm {
	inc di
	shl al,1
	loop l2

	mov cx,step_x_int
	add di,320
	sub di, step_x_int

	dec dx
	jnz l3

	pop es
	pop ds
  }

}


//вариант 2
void SCREEN::putSymbol(unsigned char s)
{
  putSymbol(x,y,s);
  x+=StepX;

//  if (x>320-8)
//  {
//    x=0;
//    y+=8;
//  }
}
//===================== end of 'putSymbol' ========================


//================= 'putText' - вывод текста =======================
void SCREEN::putText(char far * a)
{
  for(int i=0; a[i]; i++)
  {
    if (a[i] == '\n')
	{
                        x=0;
        y+=8;
            continue;
    }
    putSymbol(a[i]);
  }
}
//================== end of 'putText' ================================


//========= 'putNumb' - печатает число ==================
void SCREEN::putNumb(unsigned long n)
{
        unsigned t,k=0;
        do {
	t=n % 10 + '0';
                k++;
                asm push t
    n/=10;
		} while (n);

  for (int i=0;i<k;i++)
        {
                asm pop t
                putSymbol(t);
        }
}

//========= 'putNumb' - печатает число ==================
void SCREEN::putNumb(unsigned long n,char n1)
{
 #define k 7
 char t[k];
 if (n1>6) n1=7;
 for (int i=n1-1;i>=0;i--)
  {
    t[i]=n % 10 + 48;
    n/=10;
  }
 for (i=0;i<n1;i++)
    putSymbol(t[i]);
}

//========= 'putNumb' - печатает число ==================
void SCREEN::putBigNumb(long n)
{
 #define k 7
 char t[k];
 for (int i=k-1;i>=0;i--)
  {
	t[i]=n % 10 + 48;
	n/=10;
  }
 for (i=0;i<k;i++)
	putBigSym(t[i],color, ONE_COLOR );
}

//========= 'putNumb' - печатает число ==================
void SCREEN::putBigNumb(long n,char n1)
{
 #define k 7
 char t[k];
 if (n1>6) n1=7;
 for (int i=n1-1;i>=0;i--)
  {
    t[i]=n % 10 + 48;
    n/=10;
  }
 for (i=0;i<n1;i++)
    putBigSym(t[i], color, ONE_COLOR);
}

//=================== 'rectangle' - вывод прямоугольника ===============
void SCREEN::rectangle(RECT &rt, int color)
{
  rectangle(rt.Left, rt.Top, rt.Right-rt.Left+1, rt.Bottom-rt.Top+1,color);
}


//=================== 'rectangle' - вывод прямоугольника ===============
void SCREEN::rectangle(int x,int y,int len,int hei, int color)
{
//  if (len<1 || len>320 || hei<1 || hei >200 || x<0 || x>=320 ||
//    y<0 || y>200)
        int x1=x+len-1, y1=y+hei-1;
  if (x1<0 || y1<0 || x>319 || y>199) return;

  if (y<0 || x<0 || x1>319 || y1>199)
  {
                RECT rt1(0,0,319,199),
				rt2(x,y,x+len-1,y+hei-1),
				rt3;

    if (DetermineSystem(rt1,rt2,rt3))
                  rectangle(rt3,color);
//    Debug("SCREEN::rectangle - parametrs erorr!");
          return;
  }

  char far * vb=DoubleBuffer;
  unsigned addr=320*y+x;
  asm {
    cld
    les di,vb
    add di,addr
    mov al,byte ptr color
    mov ah,al
    mov bx,ax
    db 0x66,0xc1,0xe0,16  //shl eax,16
    mov ax,bx

	mov bx,len
    and bx,3

    mov dx,len
	shr dx,1
    shr dx,1
    cld
    }
     m:
     asm{ mov cx,dx
    db 0x66
    rep stosw
    mov cx,bx
    rep stosb
    add di,320
    sub di,len
    dec hei
    jnz m
     }
}

  int CRTCParms[]=
  {
    0x0d06,
	0x3e07,
    0x4109,
    0xea10,
    0xac11,
    0xdf12,
    0x0014,
    0xe715,
    0x0616,
    0xe317
  };



void SCREEN::Set320x240Mode()
{
  int
	SC_INDEX  = 0x3c4,
    CRTC_INDEX  = 0x3d4,
    MISC_OUTPUT = 0x3c2,
    SCREEN_SEG  = 0xa000;


  int CRT_PARM_LENGHT=10;

  graphmode();
  asm {
    mov dx,SC_INDEX
    mov ax,0x0604
    out dx,ax
    mov ax,0x0100
    out dx,ax

    mov dx,MISC_OUTPUT
    mov al,0xe3
    out dx,al

    mov dx,SC_INDEX
    mov ax,0x0300
    out dx,ax

	mov dx,CRTC_INDEX
    mov al,0x11

    out dx,al
    inc dx
    in al,dx

    and al,0x7f
    out dx,al
    dec dx

    cld
    mov si,offset CRTCParms
    mov cx,CRT_PARM_LENGHT
  }

  SetCrtLoop:
  asm {
    lodsw
    out dx,ax
	loop SetCrtLoop

    mov dx,SC_INDEX
    mov ax,0x0f02
    out dx,ax

    mov ax,SCREEN_SEG

    mov es,ax
    sub di,di
    sub ax,ax
    mov ax,12
    mov cx,0x8000
    rep stosw

	//очищаем экран
  }
}

//установка цветового регистра
void SCREEN::SetColorRegister(unsigned char n,char r,char g,char b)
{
  asm {
				mov al,n
				mov dx,0x3c8
				out dx,al

				mov dx,0x3c9

                mov al,r
                out dx,al

                mov al,g
                out dx,al

                mov al,b
                out dx,al
  }
}


//================= 'textmode'  переключение в текстовый режим ===========
void SCREEN::textmode()
{
	asm {
	mov ax,0x03
	int 0x10
	}
}


//=============== 'update' обновление части rt экрана ==============
void SCREEN::update(RECT rt)
{
 //если DoubleBuffer не существует, то ничего не происходит
  if (DoubleBuffer==VideoMemory)  return;

  char far * s=DoubleBuffer;
  char far * d=VideoMemory;
        unsigned int addr=rt.Left+rt.Top*320; //опр. смещения до точки
		int len=rt.Right-rt.Left+1;   //определяем длину переноса
        int hei=rt.Bottom-rt.Top+1;   //определяем высоту переноса
        asm {
                push es
                push ds

				lds si,s
				les di,d

				add si,addr
                add di,addr

                mov cx,len
                mov dx,hei
                mov bx,320

          cld
        }
l1:
  asm {
			   mov cx,len
                shr cx,2
                db 0x66
                rep movsw

                mov cx,len
                and cx,3
                rep movsb

				mov cx,len
                add di,bx
                sub di,cx

                add si,bx
                sub si,cx

                dec dx
                jnz l1

          pop ds
		  pop es
        }

}

//================= 'update' - обновление экрана =================
void SCREEN::update(void)
{
	//wait();
	//если DoubleBuffer не существует, то ничего не происходит
	if (DoubleBuffer==VideoMemory)  return;

	char far * s=DoubleBuffer;
	char far * d=VideoMemory;

	asm {
    push ds

    lds si,s
    les di,d

    cld

	mov cx,320*200/4

    db 0x66   //префикс заменяет 16 битные регистры на 32 битные
    rep movsw //на самом деле это movsd

    pop ds
	}
}
//================ end of 'update' ======================================


//================= 'wait' - ждет обратного хода луча ==============
unsigned long wait_flag1;
unsigned long wait_flag2;

void SCREEN::wait(void)
{
	wait_flag1=0;
	wait_flag2=0;
a:
	wait_flag1++;
	asm {
	mov dx,0x03DA
	in al,dx
	test al,8
	jnz a
	}

b:
	wait_flag2++;
	asm {
	mov dx,0x03DA
	in al,dx
	test al,8
	jz b
	}
}
//=================== end of 'wait' ===================================

SCREEN& _setlocate(SCREEN&ob, int coor)
{
  ob.y=(coor & 0xff)*8;
        ob.x=(coor>>8)*8;
        return ob;
}

SCREEN& _setcolor(SCREEN&ob, int _b)
{
  ob.color=_b;
        return ob;
}

MANIPMY manip(0,0);

MANIPMY& setcolor(int color)
{
	manip.fun=_setcolor;
	manip.ag=color;
	return manip;
}

MANIPMY& cur(int x,int y)
{
	manip.fun=_setlocate;
	manip.ag=(x<<8) + y;

	return manip;
}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//      КЛАСС <<<<<FONT>>>>>>

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

//конструктор
FONT::FONT(int size)
{
  switch (size)
  {
    case FONT8x8:
      SymHigh=8;
      break;

    case FONT8x14:
      SymHigh=14;
      break;

    case FONT8x16:
      SymHigh=16;
      break;

    default:
      fatal("FONT::FONT - illegal FONT size");
  }

  //выделяем память для фонта
  FontAddres=new char[256u * 8*SymHigh];
  if (!FontAddres) fatal("FONT::FONT - can't allocate memory");

  //инициализируем фонт
  char * sour;
        {
    asm {

      mov ah,0x11
      mov al,0x30
      mov bh,0
      push bp
      int 0x10
      mov ax,bp
      pop bp

      mov word ptr sour, ax
      mov word ptr sour+2, es
    }
  }

  for(int i=0; i<127; i++)
  {
    char * sr = sour + (i-128)*8;
    char * ds = FontAddres + i*8*SymHigh;
    for(int j=0; j<8; j++)
    {
      unsigned char byte=sr[j];
      unsigned char t=128;
      for(int v=0; v<8; v++)
      {
        if (byte & t) *ds=144+5+random(15-5);
        else *ds=0;
        ds++;
        t>>=1;
      }
    }
  }


}

FONT::~FONT()
{
  delete FontAddres;
}

void FONT::Show(int x,int y,int Symbol, char color)
{
  int hei=SymHigh;
  if (x<0 || y<0 || x>320-8 || y>200-hei) return;

  char * sourse = FontAddres + Symbol* 8*hei;
  char * dest = scr.DoubleBuffer + y*320+ x;

  asm {
      push ds
      push es

      lds si,sourse
      les di,dest

	  cld
      mov cx,hei
  }

  lp:
asm   mov ah,color
asm   mov dx,8
  df:

asm     lodsb
asm     or al,al
asm     jz lp1
asm     add al,ah
asm     mov es:[di],al
   lp1:
    asm     inc di
    asm {
      dec dx
      jnz df

	  add di,320-8
      loop lp

      pop es
      pop ds
  }

}
