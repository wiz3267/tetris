//----------------------------------
// (C) Лапшин Е.О., 1997
//----------------------------------

#define FALSE 0
#define TRUE  1

//*************************************************************
//********* SPRITE.CPP  функции-члены класса SPRITE ***********
//*************************************************************
#include <stdio.h>
#include <mem.h>
#include <fstream.h>
#include <string.h>

#include "debug.h"
#include "sprite.h"
#include "screen.h"

//инициализируем статическую переменную
int SPRITE::CountObject=0;  //число созданных объектов

// создание спрайта
// Image - адрес структуры, описывающей спрайт
int SPRITE::Create(IMAGE& Image)
{
  //проверяем, можно ли еще создать спрайт
  if ( Counter == SPRITES_MAX)
  {
    fatal("SPRITE::Create - sprites too many");
    return FALSE;
  }

  //проверяем, уникален ли индекс
  if ( find(Image.Index) != -1 )
  {
      fatal("SPRITE::Create - Index already exist");
      return FALSE;
  }

  //проверяем параметры структуры Image
  if (Image.Len > 320 || Image.Hei> 200)
  {
    fatal("SPRITE::Create - sprite too big");
    return FALSE;
  }

  //попытка выделить память под спрайт
  Image.Datacolor = new char[Image.Len * Image.Hei];
  if ( !Image.Datacolor )
  {
    fatal("SPRITE::Create - can't allocate memory");
    return FALSE;
  }

  //копируем струкутру Image в элемент Count массива структур Sprites
        int k=find_free();
  Sprites[k]=Image;
  Counter++;
  return TRUE;
}



//================== SPRITE-конструктор =====================
SPRITE::SPRITE(SCREEN far * screen, int SpMax)
{
  CountObject++;
  #if DEBUGMESSAGES
    Debug("Run SPRITE constructor");
  #endif

  //запоминаем адрес объект типа SCREEN
  scr = screen;
        //запоминаем максимальное количестов спрайтов допустимых для этого
        //объекта
        SPRITES_MAX=SpMax;

  Counter=0;  //пока число созданных спрайтов равно нулю

  //выделяем память под массив типа IMAGE
  Sprites=new IMAGE[SPRITES_MAX];

  //если не удалось выделить память
  if ( !Sprites)
  {
    CountObject--;
    fatal("SPRITE::SPRITE - no ram for IMAGE[]");
  }
        for(int i=0; i<SPRITES_MAX; i++)
        {
          Sprites[i].Datacolor=0;
        }
}

// ПОМНИ! Деструктор может быть вызван без вызова конструктора !!!
// Строй деструктор, учитывая это
//деструктор
SPRITE::~SPRITE(void)
 {
     //если программа завершилась аварийно - не был вызван конструктор
     if (CountObject!=0)
     {

  #if DEBUGMESSAGES
    Debug("Run SPRITE destructor");
  #endif


  if ( Sprites != 0 )
  {
    //удаляем память из под спрайтов
    for (int i=0; i<Counter; i++)
    {
      delete Sprites[i].Datacolor;
    }
    //удаляем память из под массива структур типа IMAGE
    delete Sprites;
  }
  else
  {
    Debug(" '~SPRITE' - Sprite==NULL! ");
  }
     }

     else //ни одного объекта не было создано
     {
  Debug("SPRITE-destructor: objects not exist");
     }

}

//поиск номера элемента массива образов спрайта с идентификатором Index
//-1 нет
//0...(SPRITES_MAX-1) номер элемента массива
int SPRITE::find(int Index)
{
  for (int i=0; i<Counter; i++)
  {
    if ( Sprites[i].Index==Index)
    {
      return i;
    }
  }

  return -1;
}

//поиск номера элемента массива, где нет спрайта
//-1 нет
//0...(SPRITES_MAX-1) номер элемента массива
int SPRITE::find_free()
{
  for (int i=0; i<SPRITES_MAX; i++)
  {
    if (Sprites[i].Datacolor==0) return i;
  }

        Debug("SPRITE::find_free - no free sprites!");
  return -1;
}


SPRITE::IMAGE far & SPRITE::GetImage(int Index)
{
  int n=find(Index);
  if (n==-1)
  {
    fatal("SPRITE::GetImage - Sprite not exist");
  }
  return Sprites[n];
}

//загрузить спрайт из файла
// fname - имя файла
// offset - смещение от начала файла
// Image - адрес структуры, описывающей спрайт
int SPRITE::Load(char far * fname, unsigned long offset, IMAGE Image)
{
  FILE * stream;

  //проверяем, можно ли еще загружать спрайт
  if ( Counter == SPRITES_MAX)
  {
    fatal("SPRITE::Load - sprites too many");
    return FALSE;
  }

  //проверяем, уникален ли индекс
  if ( find(Image.Index) != -1 )
  {
      Debug("SPRITE::Load - Index already exist");
      return FALSE;
  }

  //проверяем параметры структуры Image
  if (Image.Len > 320 || Image.Hei> 200)
  {
    fatal("SPRITE::Load - sprite too big");
    return FALSE;
  }

  //открываем файл
  stream = fopen( fname, "rb" );

  //если не смогли открыть файл
  if ( !stream)
  {
    fatal("SPRITE::Load - can't open file");
    return FALSE;
  }

  //позиционируем указатель файла
  if ( fseek( stream, offset, SEEK_SET) )
  {
    Debug("SPRITE::Load - can't seek pointer");
    fclose(stream);
    return FALSE;
  }

  //попытка выделить память под спрайт
  Image.Datacolor = new char[Image.Len * Image.Hei];
  if ( !Image.Datacolor )
  {
    fatal("SPRITE::Load - can't allocate memory");
    fclose(stream);
    return FALSE;
  }


  //читаем из файла
  fread(Image.Datacolor, Image.Len, Image.Hei, stream);

  //закрываем файл
  fclose(stream);

//  if ( l != Image.Len*Image.Hei )
//  {
//    Debug("SPRITE::Load - sprite loaded not all");
//    return FALSE;
//  }

  //копируем струкутру Image в элемент Count массива структур Sprites
  Sprites[find_free()]=Image;
  Counter++;
  return TRUE;
}


int SPRITE::LoadSpriteform(char * fname, SPRITE::IMAGE &im)
{
        ifstream ifl(fname, ios::binary);
        if (!ifl)
        {
          fatal("LoadSpriteForm - File not found");
                                  return 0;
        }


        SPRITEINFO sf;

        FILEHEADER fh;

        //считываем заголовок
        ifl.read( (char*) &fh, sizeof(fh) );

        fh.Signature[9]=0;
        if ( _fstrcmp(fh.Signature, "SPRITEFOR") )
        {
          fatal("LoadSpriteform - bad file format");
                                        return 0;
                                }

                                //позиционируемся на информационную структуру
                                ifl.seekg( fh.Info );

                                //считываем описание спрайта
                                ifl.read( (char*) &sf, sizeof(sf) );

                                //позиционируемся на массив точек
                                ifl.seekg( fh.Sprite);

        im.Len=sf.Len;
                                im.Hei=sf.Hei;
//                                im.Attribute=sf.Attribute;

                                //если спрайт создался
                                if (Create(im))
                                {
                                        //считываем сам спрайт
                                  ifl.read(im.Datacolor, im.Len*im.Hei);
                                }
                                else
        {
          fatal("Can't create the Sprite");
          return 0;

                                }
                          ifl.close();
                                return 1;

}


//прибавляет к цвету каждой точки (кроме нуля) код n
void SPRITE::ModifyColor(int index, char n)
{
  IMAGE &im=GetImage(index);
  unsigned t=0;
  for(int i=0; i<im.Len; i++)
  for(int j=0; j<im.Hei; j++)
  {
	t=im.Read(i,j);
	if (t) im.Write(i,j, t + n);
  }
}

//================= Scroll - скроллинг спрайта с возвратом ============
//Index - номер спрайта
//V - на сколько точек сдвигать
//F - набор флагов ( SCROLL_UP, SCROLL_DOWN, SCROLL_LEFT, SCROLL_RIGHT )
int SPRITE::Scroll(int Index, int F, int)
{

  int n=find(Index);
  if (n==-1) {
    Debug("SPRITE::Scroll - sprite not exist");
    return FALSE;
  }

  IMAGE & Im = GetImage(Index);

        char *Datacolor=Im.Datacolor;
        int Len=Im.Len;
        int Hei=Im.Hei;

  char *save = new char[Im.Len];

        //скроллинг вверх
  if (F & SCROLL_UP) {
    //сохраняем первую строку спрайта
    _fmemcpy( save, Im.Datacolor, Im.Len );
    //сдвигаем спрайт вверх
    _fmemcpy( Im.Datacolor, Im.Datacolor+Im.Len,
      (Im.Hei-1)*Im.Len );
    //восстанавливаем первую строку
    _fmemcpy( Im.Datacolor+(Im.Hei-1)*(Im.Len), save, Im.Len );
  }

        //скроллинг вправо
        if (F & SCROLL_RIGHT) {
          //напишем эту процедуру на ассемблере
                asm {
                        push ds
                        push es

                        lds si,Datacolor
                  les di,Datacolor

                        mov dx,Hei
                        mov cx,Len
                }

        Scroll_Loop_0:

          asm {

                        dec cx

                        add di,cx

                        mov al,es:[di]

                        mov si,di
                        dec si

      std
                        rep movsb

                        mov es:[di],al

                        mov cx,Len
                        add di,cx

                        dec dx
                        jnz Scroll_Loop_0

      pop es
                        pop ds
                }
        }

  //удаляем временный буфер
  delete save;
  return 0;
}



//================= 'ShowDirect' показывает спрайт с индексом в массиве =============
// x,y - координаты вывода спрайта
// Index - идентификатор спрайта
// Def - атрибуты для функции bitblt
// Возврат: TRUE - спрайт отображен, FALSE - отобразить спрайт не удалось
int SPRITE::ShowDirect(int x,int y,int n,int Def)
{
  if (n>=Counter || n<0)
  {
    Debug("SPRITE::ShowDirect: Index out of range");
    return FALSE;
  }

    scr ->  bitblt(x,y,
      Sprites[n].Len, Sprites[n].Hei,
      Sprites[n].Datacolor, Sprites[n].Attribute | Def);
  return TRUE;
}

//================= 'Show' показывает спрайт с индексом Index =============
// x,y - координаты вывода спрайта
// Index - идентификатор спрайта
// Def - атрибуты для функции bitblt, которые соединятся с атрибутами спрайта
// Возврат: TRUE - спрайт отобразился успешно, FALSE - невозможно отобразить
// файл
int SPRITE::Show(int x,int y,int Index,int )
{
  /*int n=find(Index);
  if (n==-1)
  {
	Debug("SPRITE::Show - sprite not exist");
	return FALSE;
  }*/

	/*scr ->  bitblt(x,y,
	  Sprites[n].Len, Sprites[n].Hei,
	  Sprites[n].Datacolor, Sprites[n].Attribute | Def);
	*/
  scr->rectangle(x,y, 8,8, 15);
  scr->x=x+1;
  scr->y=y+1;
  scr->putSymbol(Index);
  return TRUE;
}
//удалить спрайт с номером n
//????????????
int SPRITE::Delete(int Index)
{
  int n=find(Index);
  if (n==-1)
  {
    Debug("SPRITE::Delete - sprite not exist");
    return FALSE;
  }
        if (Sprites[n].Datacolor == 0)
        {
          Debug("SPRITE::Delete - you want delete space!");
                return FALSE;
        }

        delete Sprites[n].Datacolor;
        Sprites[n].Datacolor=0;
        Sprites[n].Index=-1;
        Counter--;

  return TRUE;
}

void SPRITE::IMAGE::Write(int x,int y,unsigned char color)
{
  if (x<0 || x>=Len || y<0 || y>=Hei)
        {
          Debug("IMAGE::Write - bad coordinate");
                return;
        }
        else
        {
          Datacolor[y*Len+x]=color;
        }
}

unsigned char SPRITE::IMAGE::Read(int x,int y)
{
  if (x<0 || x>=Len || y<0 || y>=Hei)
        {
          Debug("IMAGE::Read - bad coordinate");
                return 0;
        }
        else
        {
          return char(Datacolor[y*Len+x]);

        }
}