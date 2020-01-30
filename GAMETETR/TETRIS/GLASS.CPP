#include "glass.h"

#include "sprite.h"
#include "debug.h"

#include "screen.h"
#include "settings.h"
#include <fstream.h>

extern SPRITE SpriteBox;
extern SCREEN scr;

//************************************************************************
//************************ класс GLASS - стакан **************************
//*************************************************************************

GLASS::GLASS() : x(0), y(0), Len(0), Hei(0), Data(0), BoxLen(8), BoxHei(8)
{
}

//показать задний план
void GLASS::ShowBackground(int)
{
	for(int i=0; i<Hei*8; i+=20)
	for(int j=0; j<Len*8; j+=20)
	{
		//Background.Show( x+8+j, y+i, id);
	}
}

//дестркуктор
GLASS::~GLASS()
{
	//если память под стакан была выделена, удаляем ее
	if (Data) delete Data;
}

//очистка стакана
void GLASS::Clear()
{
	for(int i=0; i<Hei; i++)
	for(int j=0; j<Len; j++)
	{
		BOX f;
		f.t=0;
        f.sym=0;
		Write(j, i, f);
	}
}

//создание стакана
//len - длина
//hei - высота
void GLASS::Make(unsigned len, unsigned hei)
{
	BOX *dat = new BOX[len*hei];
	if (!dat)
	{
		Debug("GLASS::Make out of memory");
	}
	else
	{
		//если память была уже ранее выделена, освобождаем ее

		if (Data) delete Data;

		Len=len;
		Hei=hei;
		Data=dat;
		Clear();	//очищаем стакан
	}

}

//прочитать код
BOX GLASS::Read(int j, int i)
{
	BOX d;
	d.t=-1;
	d.sym=0;

	if ( j<0 || j>=Len || i>=Hei )
	{
		//Debug("GLASS::Read bad j,i");
		return d;
	}
	else if ( i<0) return d;
	{
		return Data[i*Len+j];
	}

}


extern SETTINGS Settings;

//показать стакан на экране
void  GLASS::Show()
{
	int k=1;
	if (Settings.BoxView==1) k=0;
	k=0;

	for(int i=0; i<Hei; i++)
	for(int j=0; j<Len; j++)
	{
		BOX t=Read(j,i);
		if (t.t>0)
		{
			int x1=x+8+j*BoxLen;
			int y1=y+i*BoxHei;
			scr.rectangle(x1, y1, BoxLen-k, BoxHei-k, 15);
			scr.x=x1+1;
			scr.y=y1+1;
			scr.color=0;

			scr.putSymbol(t.sym);
		}
		//if (t!=0)
		//scr.rectangle(x+8+j*8, y+i*8, 8,8, 11);
	}
	//показываем рамку вокруг стакана
	ShowBorder();

}

void graph_draw_num(int x, int y, int num);

void GLASS::ShowBorder()
{
	//int c=SPRITE_BORDER;

	/*for(int i=0; i<Hei; i++)
	 SpriteBox.Show(x, y+i*8, c);

	for(i=0; i<Hei; i++)
	 SpriteBox.Show(x+Len*8+8, y+i*8, c);

	for(i=0; i<Len+2; i++)
	 SpriteBox.Show(x+i*8, y+Hei*8, c);
	*/

	scr.border(x+8-1,y, Len*BoxLen+2, Hei*BoxHei+1, 88);


	int x1=x+8-1, y1=y+Hei*BoxHei+1;

	graph_draw_num(x1, y1, Len);
	graph_draw_num(x1+4*3, y1, Hei);

	//scr.rectangle(x+8-1,y,Len*8+2,Hei*8+1,9);

}

//записать код
void  GLASS::Write(int j, int i, BOX bx)
{
	if ( j<0 || i<0 || j>=Len || i>=Hei )
	{
		//Debug("GLASS::Write bad j,i");
		return;
	}
	else
	{
		Data[i*Len+j] = bx;
	}

}

//загрузить из файла
int	GLASS::Load(char *fname)
{
	ifstream ifl;

	//пытаемся открыть файл
	ifl.open(fname, ios::binary);

	//если нет
	if (!ifl)
	{
		return 0;
	}
	else
	{
		delete Data;

		ifl.read((char*)this, sizeof(GLASS));

		Data=new BOX[Len*Hei];

		ifl.read((char*)Data, sizeof(BOX)*Len*Hei);

		ifl.close();
	}

    return 1;
}

//сохранить в файл
int	GLASS::Save(char *fname)
{
	ofstream ofl;

	//пытаемся создать файл
	ofl.open(fname, ios::binary);
	if (!ofl) return 0;

	ofl.write((char*)this, sizeof(GLASS));
	ofl.write((char*)Data, sizeof(BOX)*Len*Hei);
	ofl.close();

	return 1;
}
