#include <stdio.h>
#include <stdlib.h>

#include "loader.h"  // loader prototypes.
#include "doscalls.h" // Include header file for access to common DOS routines.
#include "digplay.h" // Include header for DIGPAK functions.
#include "midpak.h"  // Include header for MIDPAK functions.

static int HaveDigPak=0; // True if DigPak driver currently loaded.
static int DigPakOn=0;	 // True if loaded DigPak driver is initialized.
static char far *DigPakLoad; // DigPak Load address.
static int	DigPakPara; // DigPak load at paragraph boundary.

static int HaveMidPak=0; // True if MidPak driver is loaded.
static int MidPakOn=0;	 // True if MidPak driver is initialized.
static char far *MidPakLoad; // MidPak load address.
static char far *MidPakAdv;  // MidPak ADV driver.
static char far *MidPakAd;	 // MidPak AD file.
static int	AdvPara;		 // Paragraph boundary of ADV driver.
static int	MidPakPara; // Midpak load at paragraph boundary.

unsigned char far * far memalloc(long int siz);
void far memfree(char far *memory);

int LoadDigPak(char far *fname)
{
	long int siz;

	if ( HaveDigPak ) return(0); // DigPak is allready loaded, trying to load it twice!
	DigPakLoad = floadpara(fname,&siz,&DigPakPara);
	if ( DigPakLoad )
	{
		HaveDigPak = 1; // driver was loaded. not yet initialized though.
	}
	return(HaveDigPak);
}

int InitDigPak(void)
{
	if ( HaveDigPak )
	{
		DigPakOn = InitDP(DigPakPara);
	}
	return(DigPakOn);
}

int DeInitDigPak(void)
{
	if ( DigPakOn )
	{
		DeInitDP(DigPakPara);
		DigPakOn = 0;
		return(1);
	}
	return(0);
}

int UnLoadDigPak(void)
{
	if ( HaveDigPak )
	{
		if ( DigPakOn ) DeInitDigPak();
		memfree(DigPakLoad);
		DigPakLoad = 0;
		HaveDigPak = 0;
		return(1);
	}
	return(0);
}

int LoadMidPak(char far *fname,char far *adv,char far *ad)
{
	long int siz;

	if ( HaveMidPak ) return(0); // MidPak is allready loaded, trying to load it twice!
	MidPakLoad = floadpara(fname,&siz,&MidPakPara);
	if ( MidPakLoad )
	{
		MidPakAdv = floadpara(adv,&siz,&AdvPara);
		if ( !MidPakAdv )
		{
			memfree(MidPakLoad);
			return(0);
		}
		if ( ad )
		{
			MidPakAd = fload(ad,&siz);
			if ( !MidPakAd )
			{
				memfree(MidPakAdv);
				memfree(MidPakLoad);
				return(0);
			}
		}
		else
			MidPakAd = 0;
		HaveMidPak = 1; // driver was loaded. not yet initialized though.
	}
	return(HaveMidPak);
}

int InitMidPak(void)
{
	int ret;
	if ( HaveMidPak )
	{
		ret = InitMP(MidPakPara,AdvPara,MidPakAd);
		if ( ret )
			MidPakOn = 0;
		else
			MidPakOn = 1;
	}
	return(MidPakOn);
}

int DeInitMidPak(void)
{
	if ( MidPakOn )
	{
		MidiStop(); // Stop any playing midi sequence
		DeInitMP(MidPakPara);
		MidPakOn = 0;
		return(1);
	}
	return(0);
}

int UnLoadMidPak(void)
{
	if ( HaveMidPak )
	{
		if ( MidPakOn ) DeInitMidPak();
		if ( MidPakAd ) memfree(MidPakAd);
		memfree(MidPakAdv);
		memfree(MidPakLoad);
		MidPakLoad = 0;
		HaveMidPak = 0;
		return(1);
	}
	return(0);
}

