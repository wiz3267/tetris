////////////////////////////////////////
// Copyright (C) Лапшин Евгений, 1997 //
////////////////////////////////////////

//*********************************************************************
//**** 'KEYBOARD.CPP' -функции-члены класса KEYBOARD ******************
//*********************************************************************
#include <dos.h>

enum {FALSE,TRUE };

//мои файлы
#include "process.h"
#include "debug.h"
#include "keyboard.h"
#include "screen.h"


//создаем предопределенный объект
KEYBOARD Keyb;


//==========статические (то есть в сегменте данных) переменные==============
//число созданых объектов
int KEYBOARD::CountObject=0;

 //указатель на созданный объект
KEYBOARD far * KEYBOARD::This;

char * KEYBOARD::KEY[]={
// коды 00-0F
"WHAT?","ESC", "NUM1", "NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "NUM7",
"NUM8", "NUM9", "NUM0", "DASH",
"EQUAL", "BACK SPACE","TAB",
// коды 10-1F
"Q","W","E","R","T","Y","U","I","O","P","{","}","ENTER","LEFT CTRL", "A","S",
// коды 20-2F
"D","F","G","H","J","K","L","SEMICOLON","RQUOTE", "LQUTE",
"LEFT SHIFT", "BACK SLASH", "Z","X","C", "V",
// коды 30-3F
"B","N","M", "COMMA", "PERIOD", "SLASH", "RIGHT SHIFT", "PADMUL", "LEFT ALT", "SPACE",
"CAPS LOCK", "F1", "F2", "F3", "F4", "F5",
// коды 40-4F
"F6","F7","F8","F9","F10","NUM LOCK", "SCROLL LOCK", "PAD7", "PAD8", "PAD9",
"PADSUB", "PAD4", "PAD5", "PAD6", "PADADD", "PAD1",
// коды 50-5F
"PAD2", "PAD3", "PAD0", "PADDEL","0x54", "0x55", "0x56", "F11", "F12",
"0x59", "0x5a", "0x5b", "0x5c", "0x5d", "0x5e", "0x5f",

// коды 60-6F	//мои собственные
"UP", "DOWN", "LEFT", "RIGHT", "INSERT", "DELETE", "HOME", "END","PAGE UP","PAGE DOWN",
"RIGHT ALT", "RIGHT CTRL", "PRINT SCREEN", "PAUSE", "PADENTER", "PADDIV"
};

//-------------'Check' - проверка, нажата ли клавиша -------------------
// key - скан-код проверяемой клавиши
// Возврат:
//   1 TRUE  - нажата
//   0 FALSE - не нажата
int KEYBOARD::Check(unsigned char key)
{
	return Keys[key];
}
//------------------------------------------------------------------------


int KEYBOARD::AnyKey()			// возврат 1 - если нажата any key
					//         0 - если ничего не нажато
{
	for(int i=1; i<127; i++)
	{
		if (Keyb[i]) return 1;
	}
	return 0;
}


//------------'ReadScan' - считываем скан-код ---------------------
char KEYBOARD::ReadScan(unsigned long time)
{
	//очищаем буфер клавиатуры
	Clear();

	unsigned long t=time;
	for(int i=0;;i++, i&=127)
	{
		if (time!=0)
		{	t--;
			if (t==0) break;
		}
		if (Check(i)) break;
	}

	t=time;
	while (Check(i))
	{
		if (time!=0)
		{	t--;
			if (t==0) break;
		}

	}
	return i;
}

//============== 'Clear' - очистка буфера нажатий клавиатуры ===========
//Эту функцию полезно вызывать после "потери жизни",
//Так как возможны пропуски отпусканий клавиш, и объект все время двигается
//в одну сторону
void KEYBOARD::Clear(void)
{
	for (int i=0; i<256; i++)
	{
		Keys[i]=0;
	}
}

//====================== конструктор ===================
KEYBOARD::KEYBOARD(void)
{
	#if DEBUGMESSAGES
		Debug("Run KEYBOARD constructor");
	#endif

	//объект типа клавиатуры можно создать только один раз
	if (CountObject!=0)
	{
		fatal("KEYBOARD-object already exist!");
		return;
	}

	//увеличиваем число созданых объектов
	CountObject++;

	//очищаем буфер нажатий клавиш клавиатуры
	Clear();

	//на расширенные клавиши пока не нажамали...
	ExtendedKey=FALSE;

	//сохраняем старый вектор прерывания 0x09
	int09h_old = getvect( 0x09 );

	//устанавливаем новый вектор прерывания клавиатуры
	void interrupt ( KEYBOARD::*intnew)(...) = &KEYBOARD::int09h_new;
	asm {
		cli
		xor ax,ax
		mov es,ax
		mov bx,word ptr intnew
		mov es:[9*4],bx
		mov bx,word ptr intnew+2
		mov es:[9*4+2],bx
		sti
	}

	//устанавливаем режим ввода - нет старого обработчика прерываний
	InputMode = KEYBOARD_NOOLDINTERRUPT;

	//сохраняем адрес созданного объекта.
	//он понадобится в обработчике прерывания,
	//так как ему НЕ ПЕРЕДАЕТСЯ указатель на объект this
	This=this;
}

//================= '~KEYBOARD' деструктор объекта ====================
KEYBOARD::~KEYBOARD(void)
{
	#if DEBUGMESSAGES
		Debug("Run KEYBOARD - destructor");
	#endif

	//восстанавливаем старый вектор прерывания клавиатуры
	setvect ( 0x09, int09h_old);
}

//=========== 'int09h_new' - новый обработчик прерывания клавиатуры =======
//    Поскольку это обработчик прерывания,
//обращаться к данным-членам класса просто по имени
//категорически запрещено: this принимает случайные значения,
//фактически this составляется из двух регистров процессора
//которые, разумеется, не определены при входе в обработчик
//прерывания.
//    Обработчику доступен сегмент данных, поэтому к данным-членам
//можно обратиться только посредством указателя This [на объект],
//который должен хранится в сегменте данных программы (то есть
//должен быть объявлен как static-статический )
void interrupt KEYBOARD::int09h_new(...)
{
	//объявляем переменную, в которую поместим код нажатой клавиши
	unsigned char key;

	//считываем из порта scan-код клавиши
	asm in al,0x60
	asm mov byte ptr key,al

	//если не вызывается старый обработчик прерывания,
	//то необходим стандартный ответ процессору клавиатуры
	//о том, что scan-код принят и можно реагировать
	//на следующие нажатия клавиш
	if ( This->InputMode == KEYBOARD_NOOLDINTERRUPT )
	{
		asm in al,0x61
		asm or al,0x82
		asm out 0x61,al
		asm and al,0x7f
		asm out 0x61,al
	}

	//записываем в собственный буфер факт нажатия/отпускания клавиши

	//если нажата клавиша, дающая расширенный SCAN-код
	if (key == 0xE0 )
	{
		This->ExtendedKey=0xE0;
	}
	else if ( key == 0xE1 )
	{
		This->ExtendedKey=0xE1;
	}
	//иначе нажата обычная клавиша
	else
	{
		char t;

		if (key & 0x80)           //если отпускается клавиша
		{
			t=0;
			key&=0x7f;
		}
		else	t=1;   		//иначе клавиша нажимается

		if (This->ExtendedKey == 0xE1)
		{
			if ( key == 0x1d ) {
				key=KEY_PAUSE;
				ExtendedKey=FALSE;
				}
		}

		//если нажимается расширенная клавиша
		//проводим тополнительную обработку
		if (This->ExtendedKey == 0xE0)
		{
			//проводим дополнительный анализ кодов клавиш
			switch (key) {
				case KEY_PAD0:	key=KEY_INSERT; break;
				case KEY_PAD1:	key=KEY_END;    break;
				case KEY_PAD2:	key=KEY_DOWN;   break;
				case KEY_PAD3:	key=KEY_PAGEDOWN;break;
				case KEY_PAD4:	key=KEY_LEFT;   break;
				case KEY_PAD6:	key=KEY_RIGHT;  break;
				case KEY_PAD7:	key=KEY_HOME;   break;
				case KEY_PAD8:	key=KEY_UP;     break;
				case KEY_PAD9:	key=KEY_PAGEUP; break;

				case KEY_ENTER: key=KEY_PADENTER;     break;
				case KEY_LEFTCTRL: key=KEY_RIGHTCTRL; break;
				case KEY_LEFTALT:  key=KEY_RIGHTALT;   break;
				case KEY_PADDEL:   key=KEY_DELETE;	break;
				case 0x35:	   key=KEY_PADDIV;	break;
//				case KEY_LEFTSHIFT:	key=KEY_PRINTSCREEN; break;

				default:	key=FALSE; break;
			} //конец switch
		}

			This->ExtendedKey=FALSE;

		if (key)
		{
                        if ( (t==1 && This->Keys[key]==0) || t==0)
				This->Keys[key]=t;
                }
	}

	//если надо использовать стандартный обработчик
	//то вызываем его
	if ( This->InputMode & KEYBOARD_YESOLDINTERRUPT)
	{
		This->int09h_old();	//не забываем обращаться
						//к данным через указатель
		return;
	}

	//если стандартный обработчик не вызывается
	//то необходимо подать контроллеру прерываний
	//команду "конец прерывания".
	//  В противном случае на запросы прерывания >=IRQ 2
	// контроллер прерываний реагировать не будет: система 'зависнет'
		asm mov al,0x20		//посылаем контроллеру прерываний команду
		asm out 0x20,al		// 'конец прерывания'

	if ( This->Keys[ KEY_LEFTCTRL] && This->Keys[KEY_LEFTALT] &&
	 This->Keys[ KEY_DELETE] )
	{
		exit(1);
	}
}

void KEYBOARD::Set(char keycode,char what)
{
	Keys[keycode & 0x7f]=what;
}

//========== 'SetInputMode' - установка режима ввода с клавиатуры ========
// 'mode' - режим:
//    KEYBOARD_YESOLDINTERRUPT - означает, что кроме нового обработчика
// прерывания 0x09 будет вызываться старый, то есть имеется возможность
// вызывать все функции DOS & BIOS для клавиатуры
//    KEYBOARD_NOOLDINTERRUPT - будет действовать только новый обработчик
//  функции DOS & BIOS для клавиатуры вызывать нельзя
//  Возврат:
//	текущий режим
int KEYBOARD::SetInputMode(int mode)
{
	int temp=InputMode;
	InputMode=mode;
	return temp;
}

void KEYBOARD::SuperInput()
{
	SetInputMode(KEYBOARD_NOOLDINTERRUPT);
}

void KEYBOARD::StandartInput()
{
	SetInputMode(KEYBOARD_YESOLDINTERRUPT);
}