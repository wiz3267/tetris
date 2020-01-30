//---------------------------------------
// Copyright (C) Лапшин E.O., 1997
//---------------------------------------


//*******************************************************************
//***************** функции-члены класса 'SOUND' ********************
//*******************************************************************
#include <process.h>
#include <stdlib.h>
#include <stdio.h>

//#include <iostream.h>
//#include <conio.h>
//#include <alloc.h>

#include "debug.h"
#include "sound.h"
#include "midpak2.h"
#include "doscall2.h"
#include "loader2.h"

#include "settings.h"
extern SETTINGS Settings;
int SOUND::CountObject=0;

//конструктор
SOUND::SOUND(void)
{
	CountObject++;

	#if DEBUGMESSAGES
		Debug("Run SOUND constructor");
	#endif


	int yes;

//Вызов загрузчика для начальной загрузки стандартного драйвера DIGPAK
	if ( ! (DIGPAK=LoadDigPak("driver\\SOUNDRV.COM")) )
		fatal("Failed to load sound driver.");
//В случае неудачной инициализации завершаем программу
	if ( ! (INITDIGPAK=InitDigPak()) );
//		Debug("Failed to initialize sound driver.");


//Загрузка MIDPAK

	if ( ! (MIDPAK=LoadMidPak("driver\\MIDPAK.COM", "driver\\MIDPAK.ADV", "driver\\MIDPAK.AD")) )
		fatal("Failed to load MIDPAK driver.");
	if ( ! (INITMIDPAK=InitMidPak()) );
//		Debug("Failed to initialize MIDPAK driver.");

	if (  ! (DIGPAKAVAILABLE=DigPakAvailable()) );
//	Debug("DIGPAK not available from MIDPAK!");

	asm {
	xor ax,ax
	mov es,ax
	mov ax,es:[0x66*4+2]
	mov yes,ax
	}
	if ( !yes )
        	fatal("DIGPAK not loaded!");
	count=0;
}
//============ 'soundStatus' ================================
int SOUND::soundStatus(void)
{
  if ( DIGPAK && INITDIGPAK && DIGPAKAVAILABLE)
  {
	asm {
	mov ax,0x689
	int 0x66
	}
   return _AX;
  }
 return 0;
}


//=============== SndPlay - проиграть звук ===================
void SOUND::SndPlay(SNDSTRUCT far & sndplay)
{
   if ( DIGPAK && INITDIGPAK && DIGPAKAVAILABLE)
   {
	int yes;
	asm {
	xor ax,ax
	mov es,ax
	mov ax,es:[0x66*4+2]
	mov yes,ax
	}
	if ( !yes )
		fatal("DIGPAK not loaded!");

	asm {
	push ds
	mov ax,0x688
	lds si,sndplay
	int 0x66
	pop ds
	}
    }
}
//============= end of 'SndPlay' ==============================



//============= 'play' - проиграть звук n =============
void SOUND::play(int n,int flag)
{
	if (Settings.EnableSound==0) return;
        //если текущий звук проигрывается в режиме "не останавливать"
        //и сейчас проигрывается другой звук - остановить проигрывание
	if (sounds[current].flag == NOSTOP && soundStatus()==YESPLAYING) return;

	if (n<0 || n>count-1 || count==0)
	{
                //WARNING ! This is only on some-time !
		//fatal ("SOUND::play - cannot play not exist a sound!");
        	return;
        }

	sounds[n].flag=flag;
	current=n;
	stop();	//останавливаем текущий звук
	SndPlay(sounds[n]);
}
//============== end of 'play' ==================================


//============= 'stop' - остановить текущий звук =========
void SOUND::stop(void)
{
   if ( DIGPAK && INITDIGPAK && DIGPAKAVAILABLE)
   {
	asm mov ax,0x68F
	asm int 0x66
   }
}

//============ 'load' - загрузить звук в память ===============
int SOUND::load(char far * fname,int FreqDefault)
{
 FILE * stream;
 long file_size;
 if (count == count_max) { fatal ("SOUND::load - cannot load over 'cound_max' sounds!"); return -1; }

 // открываем файл и определяем его длину
 if ( (stream = fopen(fname, "rb")) != NULL )
   {
     fseek(stream, 0L, SEEK_END);
     fgetpos(stream, &file_size);
     fseek(stream, 0L, SEEK_SET);
   }
 else fatal ("SOUND::load - cannot open file!");

 //временно !!!
// file_size = 12000;

 if (file_size>65535u) fatal("SOUND::load - length of file >65535");
 // выделяем память под звук и заполняем структуру SNDSTRUCT
//  sounds[count].sound=(unsigned char *) farmalloc(file_size);
  sounds[count].sound=new char[file_size];
//  sounds[count].sound=new char[file_size];
 if (sounds[count].sound == NULL)
   fatal("SOUND::load - not enough memory for load sound");

 // читаем из файла
 fread(sounds[count].sound, 1, file_size, stream);
// if (st!=1) fatal("SOUND::load - file loading error");

 for(long i=0; i<file_size; i++)
 {
   int a=sounds[count].sound[i];
   a-=128;
   a/=8;
   a+=128;
   sounds[count].sound[i]=a;

 }

 //закрываем файл
 fclose(stream);

 sounds[count].sndlen=file_size;
 sounds[count].frequency=FreqDefault;		//частота по умолчанию
 sounds[count].IsPlaying=&isplay;	//адрес флага
 sounds[count].flag=0;
 count++;
 return count;
}
//================ end of 'load '========================================

//=============== 'unload' - выгрузить звук n из памяти =============
void SOUND::unload(int n)
{
	if (n<0 || n>count-1 || count==0) fatal ("SOUND::unload - cannot unload not exist a sound!");
	delete sounds[n].sound;
}
//================= end of 'unload' =====================


//================ деструктор ============================
SOUND::~SOUND(void)
{
    if ( CountObject != 0 )
    {
	#if DEBUGMESSAGES
		Debug("Run SOUND destructor");
	#endif


//выгрузка драйверов
	DeInitMidPak();
	UnLoadMidPak();

	DeInitDigPak();
	UnLoadDigPak();

	for (int i=0; i<count; i++)
		unload(i);
    }

    else	//ни одного объекта не было создано
    {
	#if DEBUGMESSAGES
		Debug("SOUND-destructor: obj no exist!");
	#endif
    }

}
//================= end of 'деструктор' ===================