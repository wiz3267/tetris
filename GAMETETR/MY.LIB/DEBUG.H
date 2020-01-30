//
#ifndef __DEBUG_H
#define __DEBUG_H
//*********************************************************
//**** Прототипы функции для DEBUG.CPP ********************
//*********************************************************

#define TRUE  1

#define FALSE 0

//================= 'Debug' - выдача сообщение об ошибке =================
void Debug(char far *);
void Msg(char far *,int=12);	//выдача сообщения
int MsgText(char far *msg, char far * text1, char far * text2, int bool_init, int y=0);
void MsgText(char far *msg, char far * text1, char far * text2, int * bool_init, int y_coor);

int MsgTextMinMaxValue(char far *msg, int min_value, int max_value, int current_value, int y_coor);

//== 'fatal' - выдача сообщения об ошибке и завершение работы программы ===
void fatal(char far * str);
//================= 'Debug' - выдача сообщение об ошибке =================
#endif