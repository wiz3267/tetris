// (c) 1997 E.LAPSHIN

#include "bmpview.h"

#include <conio.h>
#include <fstream.h>
#include "debug.h"

#include "screen.h"


extern SCREEN scr;

extern "C" void exit(int);

// устанавливает цветовой регистр с номером n (0..255) в соответствием со
// значениями r,g,b
void SetColorRegister(char r, char g ,char b , char n)
{
	scr.SetColorRegister(r,g,b, n);
}


void page(int ,char )
//void page(int N,char AorB=0)
{
	return;
	/*
	asm mov ax,0x4F05
	asm mov bh,0
	asm mov bl,AorB
	asm mov dx,N
	asm int 0x10*/
}




//char far *VideoMemory=(char far *)0x0A0000000L;

//int *ivmem=(int *)0xA0000000L;

//void putpixel(int x,int y,char r,char g,char b)
void putpixel(int ,int ,char ,char ,char )
{
  /*	int k=(r<<10)+(g<<5)+b;
	unsigned long offset=y*640L*2+x*2;

	page(offset/65536L);
	ivmem[(offset%65536L)>>1]=k;
  */
}

void putpixel(int x,int y,char color)
{
	if ( x>320 || y>200 || x<0 || y<0) return;

    scr.putpixel(x,y, color);
	/*asm {
			les di,VideoMemory
				mov ax,y
				shl ax,8
				add di,ax

				mov ax,y
				shl ax,6
				add di,ax

				add di,x
				mov al,color
				stosb
		}
	*/
}


void fatal_(char *)
{
  //	cout<<endl<<"----- "<<s<<" -----"<<endl;
  //	exit(-1);
}

char ArrayScanLine[200][320];

char * LoadBMP320x200x256(char *fname)
{
		ifstream ifl(fname, ios::binary);

		if (!ifl)
		{
			Msg("BMPVIEW - file not found");
			return NULL;
		}

		BITMAPFILEHEADER bh;

		//считываем заголовок bmp-файла
		ifl.read( (unsigned char*) &bh, sizeof(bh) );
		long FileSize = bh.bfSize;

		//проверяем, это bmp-файл ?
		if ( bh.bfType != 0x4D42   ||  bh.bfSize != FileSize)
		{
			Msg("BMPVIEW - this is not bmp-file");
			return NULL;
		}

		//по видимому, это bmp-файл

		//продолжаем читать файл
		BITMAPINFOHEADER info;
		ifl.read( (char*) &info, sizeof(info) );

		//обрабатываем ошибочные ситуации
		if ( info.biPlanes != 1  )	Msg("BMPVIEW - biPlanse NOT EQU 1L");
		if ( info.biSize   != 40 )	Msg("BMPVIEW - biSize NOT EQU 40L");
		unsigned long t=info.biBitCount;

		if ( ! (t==1 || t==4 || t==8 || t==24) )
		{
			Msg("BMPVIEW - biBitCount NOT EQU 1,4,8 or 24");
			return NULL;
		}

		if ( t==24)
		{
			//mode(0x110);
			Msg("BMPVIEW - True-color is NOT SUPPORTED");
			return NULL;
		}

		//проверяем, сжато ли изображение
		if ( info.biCompression != BI_RGB )
		{
				//если да, то выдаем сообщение, что эта программа
				//не поддерживает сжатые bmp-файлы
			Msg("BMPVIEW this programm does not support COMPRESSED file");
		}


		//определяем число элементов bmiColor
		if ( t==1 ) t=2;
		else if ( t==4 ) t=16;
		else if ( t==8 ) t=256;
		else if ( t==24) t=0;

        //если есть палитра
		if (t)
		{
			//выделяем память под массив цветов
			RGBQUAD *clr = new RGBQUAD[t];
			if (!clr)
			{
				Msg("BMPVIEW - Can't allocate memory for data of colors!");
			}

			//считываем палитру
			ifl.read( (char*) clr, sizeof(clr)*t );

			//загружаем регистры палитры
			for(int i=0; i<t; i++)
			{
			SetColorRegister(i, clr[i].rgbRed*63/255,
			 clr[i].rgbGreen*63/255, clr[i].rgbBlue*63/255);
			}

			delete clr;
		}

		//вычисляем длину строки
		int LineLen = info.biWidth * info.biBitCount / 8;

		ifl.seekg(bh.bfOffBits);

		t=0;


	for(int i=199; i>=0; i--)
	{
		//считываем строку в память
		ifl.read( (char*) ArrayScanLine[i], 320);
	}

	ifl.close();

	return (char*)ArrayScanLine;
}

void DrawBMP(int start_y, int hei)
{
	//вычисляем длину строки
	int LineLen = 320;

	//цикл рисования изображения
	int line_count=0;

	for(int i=0; i<200; i++)
	{

		int y=i+start_y;
		if (y>200) break;

		scr.bitblt(0,y,320,1, ArrayScanLine[i]); //бит-блитинг

		line_count++;

		if (line_count == hei) break;

	}
}

