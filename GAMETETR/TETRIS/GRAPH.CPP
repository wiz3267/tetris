#include <string.h>
#include <stdio.h>

#include "screen.h"

void Pixel(int x, int y, int color)
{
	scr.putpixel(x,y,color);
}

void setpixel(int x, int y, unsigned char color)
{
	int scale=1;
	
	for(int i=0; i<scale; i++)
		for(int j=0; j<scale; j++)
		{
			Pixel(x*scale+j,y*scale+i, color);
		}
}

char * digits[]=
{
		"1111"
		"1001"
		"1001"
		"1001"
		"1111",
		
		"0011"
		"0001"
		"0001"
		"0001"
		"0001",

		"1111"
		"0001"
		"1111"
		"1000"
		"1111",

		"1111"
		"0001"
		"1111"
		"0001"
		"1111",

		"1001"
		"1001"
		"1111"
		"0001"
		"0001",

		"1111"
		"1000"
		"1111"
		"0001"
		"1111",

		"1111"
		"1000"
		"1111"
		"1001"
		"1111",

		"1111"
		"0001"
		"0001"
		"0001"
		"0001",

		"1111"
		"1001"
		"1111"
		"1001"
		"1111",

		"1111"
		"1001"
		"1111"
		"0001"
		"1111"
};


//_____________________________________________________________________________
void graph_draw_digit(int x, int y, int n)
{
	int x1=0, y1=0, yi, xi;
	if (n>=10) return;

	for(x1=x, xi=0; xi<4; x1++, xi++)
		for(y1=y, yi=0; yi<5; y1++, yi++)
		{
			if (digits[n][yi*4+xi] == '1')
			{
				setpixel(x1,y1,scr.color);
			}
		}
}

//_____________________________________________________________________________

char symbol_dvoe[]=
"0000"
"0110"
"0000"
"0110"
"0000";


char symbol_point[]=
"0000"
"0000"
"0000"
"0011"
"0011";

//_____________________________________________________________________________
char symbol_minus[]=
"0000"
"0000"
"1111"
"0000"
"0000";


int scalex_text=1;
int scaley_text=1;

void graph_draw_symbol(int , int , char , int )
{
  //преобразуем кодировку
/*
  char zz[2]={s,0};
  CharToOem(zz,zz);
  s=zz[0];

  unsigned char *sour=Font8x8;


  sour+=s*8;


  for(int i=0; i<8; i++)
  {
	  char a=sour[i];
	  char t=1;
	  for(int j=7; j>=0; j--)
	  {
		for(int v=0; v<scaley_text; v++)
		{
			if (a & t)
			{
				for(int k=0; k<scalex_text; k++)
				Pixel(x+j*scalex_text+k, y+i*scaley_text+v, color);
			}
		}

		t<<=1;
	  }
  }
*/
}

void graph_draw_text(int x, int y, char * str, int clr)
{
	for(int i=0; i<strlen(str); i++)
	{
		graph_draw_symbol(x+i*8*scalex_text, y, str[i], clr);
	}
}


//_____________________________________________________________________________
void graph_draw_image(int x, int y, int xl, int yl, char *img)
{
	int x1,y1, xi, yi;

	for(x1=x,xi=0; x1<x+xl; x1++, xi++)
		for(y1=y,yi=0; y1<y+yl; y1++, yi++)
		{
			if (img[yi*xl+xi] == '1')
			{
				setpixel(x1,y1,scr.color);
			}

		}
}

//_____________________________________________________________________________
void graph_draw_symbol(int x, int y, char sym)
{
	if (sym>='0' && sym<='9')
	{
		graph_draw_digit(x, y, sym-'0');
	}
	else if (sym=='.')
	{
		graph_draw_image(x, y, 4, 5, symbol_point);
	}
	else if (sym=='-')
	{
		graph_draw_image(x, y, 4, 5, symbol_minus);
	}

	else if (sym==':')
	{
		graph_draw_image(x, y, 4, 5, symbol_dvoe);
	}


}

//_____________________________________________________________________________
void graph_draw_num(int x, int y, int num)
{
	char buf[32];
	int i=0;
	sprintf(buf, "%i", num);
	
	for(i=0; i<strlen(buf); i++)
	{
		graph_draw_symbol(x+i*5, y, buf[i]);
	}
}

//_____________________________________________________________________________
void graph_horz_line(int x, int y, int len)
{
	int i=0;
	do {
		setpixel(x+i,y,scr.color);
		len--;
		i++;
	} while (len);
}

//_____________________________________________________________________________
void graph_vert_line(int x, int y, int len)
{
	int i=0;
	do {
		setpixel(x,y+i,scr.color);
		i++;
		len--;
	} while (len);
}

