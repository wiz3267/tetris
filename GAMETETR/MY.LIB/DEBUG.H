//
#ifndef __DEBUG_H
#define __DEBUG_H
//*********************************************************
//**** ���⨯� �㭪樨 ��� DEBUG.CPP ********************
//*********************************************************

#define TRUE  1

#define FALSE 0

//================= 'Debug' - �뤠� ᮮ�饭�� �� �訡�� =================
void Debug(char far *);
void Msg(char far *,int=12);	//�뤠� ᮮ�饭��
int MsgText(char far *msg, char far * text1, char far * text2, int bool_init, int y=0);
void MsgText(char far *msg, char far * text1, char far * text2, int * bool_init, int y_coor);

int MsgTextMinMaxValue(char far *msg, int min_value, int max_value, int current_value, int y_coor);

//== 'fatal' - �뤠� ᮮ�饭�� �� �訡�� � �����襭�� ࠡ��� �ணࠬ�� ===
void fatal(char far * str);
//================= 'Debug' - �뤠� ᮮ�饭�� �� �訡�� =================
#endif