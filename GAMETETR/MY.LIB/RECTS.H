#ifndef __RECTS_H
#define __RECTS_H

#define BIG_HORZ 0x02	//больше горизонтальная
#define BIG_VERT 0x04	//больше вертикальная

//---------- 'RECT' - описывает прямоугольную область -------------
struct RECT {

public:
    RECT(int x1,int y1,int x2,int y2) :
    	 Left(x1), Top(y1), Right(x2), Bottom(y2) {;}
    RECT() {;}
    void Set(int x,int y,int x1,int y1) { Left=x,Top=y,Right=x1,Bottom=y1; }
    int Left;
    int Top;
    int	Right;
    int	Bottom;
};
//-------------------------------------------------------------------


int CheckUnionRects(RECT far & rect1, RECT far & rect2);
int DetermineSystem(RECT&,RECT&,RECT&);

#endif