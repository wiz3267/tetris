#ifndef __MOVEOBJ_H
#define __MOVEOBJ_H

#include "rects.h"
#include "debug.h"


class SPRITE;		//�।���⥫쭮� ������� �����


class MOVEOBJ
{
	friend LIST;

	//YES - ����� �㯥ࢨ���, NO - ����� ��ꥪ�
public:	enum { YES=1, NO=-1 };	//���祭�� ��६����� Supervisor

//*********************** �����-������ ************************
protected:
static	int SupervisorCounter;//�᫮ ᮧ������ ��ꥪ⮢-�㯥ࢨ��஢
	SPRITE* Sprite;		//㪠��⥫� �� ��ꥪ� ⨯� SPRITE
	int	Supervisor;	//䫠� �㯥ࢨ���
	MOVEOBJ* First;   	//㪠��⥫� �� ���� ��쥪� c��᪠
	MOVEOBJ* Last;		//㪠��⥫� �� ��᫥���� ��ꥪ� ᯨ᪠

public:

	static	int TotalAnimatedObject;
	float	Acceleratx;	//�᪮७�� �� ���
	float	Acceleraty;
	int	Counter;	//�᫮ ᮧ������ ��ꥪ⮢ � ᯨ᪥
	RECT	Hot;		//��।���� ����祥 ��⭮ �ࠩ�
	MOVEOBJ* Next;		//㪠��⥫� �� ᫥���騩 ��쥪�. �᫨ �� ��᫥���� ����� Next=NULL
	MOVEOBJ* Prev;		//㪠��⥫� �� �।��騩 �����.�᫨ �� ���� ��쥪� Prev=NULL
	int	Link;		//�ਢ離� ��ꥪ�

	static	RECT	Lim;		//�࠭���, �멤� �� ����� ��ꥪ�� 㤠������
	RECT	Rect;		//��।���� ��ꥪ� �� ��࠭�
	float	Speedx;		//�஥�樨 ᪮��� �� ��
	float	Speedy;
	int	SpriteIndex;	//������ �ࠩ�
	float	Speed;		//����� ����� ᪮���
	int	Status;		//���ﭨ� ��쥪�

	static	float	SPEED_UNIT;	//������ ᪮���
	static	int	WindowX;	//���न��� X ���� �뢮��
	static	int	WindowY;	//���न��� Y ���� �뢮��

	float	x,y;		//⥪�騥 ���न���� ��쥪�
	int	ShootCounter;	//�᫮ ����५��, ��᫥ 祣� ��ꥪ�
	int	AlwaysAnimate;	//�ᥣ�� �� ���� �����஢��� ��ꥪ�� ?
				//㡨������
//********************** �������-����� ***************************
//(����⪠ YES - ����� �㭪�� �।�����祭� ⮫쪮 ��� �㯥ࢨ���
// NO - �㭪�� �� �।�����祭� ��� �㯥ࢨ���, � ���� �।�����祭�
// ⮫쪮 ��� ��ꥪ�)
public:
	void Animate();				//YES ������� ����⮢
virtual	void Del (MOVEOBJ *Obj);		//YES 㤠����� ��쥪� �� ᯨ᪠ �� ��� �����,
	void Del (int n) { Del(&operator[](n));}//YES 㤠����� ��ꥪ� � ����஬ n
	void DelAll();				//YES 㤠����� ��� ��ꥪ⮢
	virtual void Destroy(int who) {}		//��뢠���� ����� ��ꥪ� ��-� 㡨����
	void MakeSupervisor(SPRITE*spr)		//������ ��ꥪ� �㯥ࢨ��஬
	{
		Supervisor=YES;
		Sprite=spr;
	}
	 MOVEOBJ(SPRITE *Sprite);		//YES ��������� �㯥ࢨ���
	 MOVEOBJ();				//NO ��������� move-��ꥪ�
	~MOVEOBJ();                     	//YES, NO ��������
	int  New( MOVEOBJ *Obj, int size);	//YES ᮧ����� ������ ��ꥪ�
	MOVEOBJ& operator[] (int i);		//YES ������� ��뫪� �� ��ꥪ� i
	void operator+= (MOVEOBJ &);		//YES �������� ��ꥪ� � ᯨ᮪
	virtual void RenewFrame() {}
	void RenewRect();			//NO  �������� �������� Rect
	virtual int RenewTimeLive() {return 1;}
	virtual	void RenewXY();				//NO  �������� ���न���� X,Y
	//NO ��⠭����� �������� ����� ��६����
	void Set(float x=0,float y=0,float sx=0,float sy=0,float acx=0,float acy=0);
	virtual	void Show();				//NO �����뢠�� ��ꥪ�
	void ShowAll();				//YES �������� �� ��ꥪ��
};


//****************************************************
//************* class ANIM - ������� ��ꥪ⮢ *******
//****************************************************
//����� �����ਢ��� MOVEOBJ ⠪�� ��ࠧ��, �� ���� ��ꥪ��
//����� ᠬ�㭨�⮦����� (��६����� TimeLive) � ��������
//��᫥����⥫쭮 c �㦭�� ᪮����� ����� �ࠩ⮢ (� ��।�������
//��१�� ���祭��)

class ANIM: public MOVEOBJ
{
public:
	int FrameFirst, FrameLast;
	float FrameSpeed, FrameCurrent;
	int TimeLive;

	unsigned char Color;
	float BoxLen;
	float BoxHei;

public:

	void Show();
	ANIM ();
	ANIM(SPRITE *s) : MOVEOBJ(s) {;}
	//��ॣ�㦠�� [] ��� �ࠢ��쭮�� �८�ࠧ������ ⨯�
	ANIM & operator[] (int i) {return (ANIM&) MOVEOBJ::operator[](i); }
	//�������, MOVEOBJ & operator+= ⠪�� ��⠥��� !
	void operator+= (ANIM& ob) { New(&ob, sizeof(ob)); };
	void Animate(void);
	virtual	void RenewFrame();

	int RenewTimeLive()
	{
		if (TimeLive>3)
		{
			TimeLive--;
			return 1;
		}
		else if (!TimeLive) return 1;
		else return 0;
	}

};


//************************************************************
//************* class MINDOBJ - 㬭�, ࠧ㬭� ��ꥪ�� *******
//************************************************************
//����� ANIM �������� ᮧ������ ���⥩��� �������. �᫨ ��� �� ���ࠨ����
//���������� ����� ANIM, �ᯮ���� ����� MINDOBJ

//0,-1,10, 0,1,10, 0	���⨪ �����, ����
//

class MINDOBJ : public ANIM
{
public:
        //������� "��졠" (����? - �᭮,�� �� ���)
        struct GOING
	{
		float Speed_x,Speed_y;	//��६�饭�� �� ��� �� ���� 蠣
//               	Accelerat_x,Accelerat_y;
		int  Time;	    //�६� �⮣� ��।�������(�᫮ 蠣��)
        			   //�᫨ Time==0, ����� ��� 蠣 ��᫥����
        };
        //��� ������ �������� 䠧�
        struct FRAMING
        {
        	int Fase,	//����� 䠧�
                    Time;	//᪮�쪮 �६���� ��� �㤥� ��ঠ����
                    		//�������筮 GOING, �᫨ Time==0 - �� ��砫�
	};

	GOING  * going;		//㪠��⥫� �� ᯮᮡ ��������
        FRAMING * framing;        //㪠��⥫� �� ᯮᮡ ᬥ�� 䠧��

        int go_num, frame_num;	//⥪�騥 ��������
        int go_max, frame_max;	//�᫮ ������⮢

public:
        GOING  Go;		//⥪�騥 ��������
	FRAMING Frame;


        //�������
        enum {
		SET_SPEED	= -1,	//��⠭����� ᪮����
		SET_ACCELERAT   = -2,	//��⠭����� �᪮७��
                SET_INDEX	= -3	//��९�룭��� �� ��㣮� �������
	};

        enum {
             PERSON=0x01,      //��ꥪ� �ᥣ�� ᬮ��� � ��஭� ��६�饭��
                               //(���� �⮡ࠦ����� ⠪, ��� ���ᮢ��)
             DANGER=0x02,	//��ꥪ� �����
	     			//(���� �������)
             SOLID=0x04,	//��ꥪ� ⢥��(����� �ன� �१ ����)
             			//(���� �஧���)
             LIFT=0x08,		//��ꥪ� ����� ��७���� �� ᥡ�(���⨪,
	             		//⥫����, ����)
        	                //(���� �� �����)
        };

        unsigned State;		//���ﭨ� �������

        void Add(MINDOBJ&, int NumGo,GOING *go_ptr, int NumFrame,
	FRAMING *frame_ptr,int StartGo=0, int StartFrame=0);


	void Del (MOVEOBJ *Obj);	//㤠����� ��ꥪ� �� ᯨ᪠
	MINDOBJ(SPRITE *s) : ANIM(s) {;}
        MINDOBJ();
        ~MINDOBJ();         //��������
  	void operator+=(MINDOBJ&);
        MINDOBJ& operator[](int i) { return (MINDOBJ&) MOVEOBJ::operator[](i); }
        void RenewFrame();
        void RenewXY();
        void Show();

};



//��६���騨�� ⥪��
class MOVETXT : public ANIM
{
        public:
	char* Message;		//ᮮ�饭��
        char Color;
        MOVETXT (SPRITE* spr) : ANIM(spr), Message(0) {}
	MOVETXT () : Message(0), Color(15) {;}
	void Animate();
	void operator+= (MOVETXT& ob) { New(&ob, sizeof(ob)); };
	void Show();
};

/*
class LIST {

	MOVEOBJ* First;   	//㪠��⥫� �� ���� ��쥪� c��᪠
	MOVEOBJ* Last;		//㪠��⥫� �� ��᫥���� ��ꥪ� ᯨ᪠
        SPRITE * Sprite;	//㪠��⥫� �� ���ਯ�� �ࠩ⮢
        			//����� ���� �ᯮ�짮������
                                //�� �⮡ࠦ���� ��ꥪ⮢ ������� ᯨ᪠

public:
        int Counter;		//�᫮ ��ꥪ⮢ � ᯨ᪥

	void Animate();				//������� ����⮢
	void  Del (MOVEOBJ *Obj);		//㤠����� ��쥪� �� ᯨ᪠ �� ��� �����,
	void  Del (unsigned n) { Del( (MOVEOBJ*)Get(n)); }
	void  DelAll();
	void* Get (unsigned n);
//        void Del (int n) { Del(&operator[](n));}//㤠����� ��ꥪ� � ����஬ n
//	void DelAll();				//㤠����� ��� ��ꥪ⮢ ᯨ᪠
	LIST(SPRITE *Sprite);			//��������� ᯨ᪠
	~LIST();				//�������� ᯨ᪠
//inline  int  Add( MOVEOBJ &obj) { return Add(&obj); }
//        int  Add( MOVEOBJ *obj);		//�������� ��ꥪ� � ᯨ᮪
	int  Add( MOVEOBJ *obj, int size);	//���������� ��ꥪ� � ᯨ᮪
        //�� ��������!

//	MOVEOBJ& operator[] (int i);		//������� ��뫪� �� ��ꥪ� i
//	void operator+= (MOVEOBJ &);		//�������� ��ꥪ� � ᯨ᮪
//	void ShowAll();				// �������� �� ��ꥪ�� � ᯨ᪥

};

template <class T>
void operator+=(LIST &list, T& obj)
{
	list.Add(&obj, sizeof(obj) );
}

/*
template <class T>
T& operator[](unsigned i)
{
return (T&) (*Get(i));
}*/


#endif