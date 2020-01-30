#include "SCREEN.H"
#include "KEYBOARD.H"

extern SCREEN scr;
extern KEYBOARD Keyb;

void TetrisHelp()
{
		scr.color=15;
		scr.clear();
		char * help=
		"F1 помощь\n"
		"F2 следующая музыка\n"
		"F5 эффекты\n"
		"F9 информация \n"
		"F10 часы\n"
		"F11 таблица рекордов \n"
		"F12 бегущая строка\n"
		"<PAD4> следующая глава\n"
		"<PAD6> предыдущая глава\n"
		"<PAD1> случайная часть\n"
		"<PAGEUP><PAGEDOWN> размер бегущей строки\n"
		"<LEFTCTRL> <+><->  ширина ячейки\n"
		"<LEFTSHIFT> <+><-> высота ячейки\n"
		"<LEFTCTRL> <1><2> ширина стакана\n"
		"<LEFTCTRL> <3><4> высота стакана\n"
		;

		scr.x=0;
		scr.y=0;
		scr<<help;

		scr.update();

		Keyb.ReadScan();
}