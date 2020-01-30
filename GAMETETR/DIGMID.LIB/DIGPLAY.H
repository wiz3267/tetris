/*****************************************************************************/
/* DIGPLAY.H	C prototype header for the DIGPAK API link layer							 */
/*						DIGPLAY.ASM.	Because sound cards playback sound via DMA			 */
/*						addresses passed must be in the first 1mb of address space. 	 */
/*																																					 */
/*						See DIGPKAPI.DOC for complete DIGPAK documentation. 					 */
/*																																					 */
/*																																					 */
/*		Written by John W. Ratcliff (c) 1994																	 */
/*			 Compuserve: 70253,3237 																						 */
/*			 Genie: J.RATCLIFF3 																								 */
/*			 BBS: 1-314-939-0200																								 */
/*			 Addresss:																													 */
/*					 747 Napa Lane																									 */
/*					 St. Charles, MO 63304																					 */
/*																																					 */
/*		A $500 per product license fee applies to all commercial software 		 */
/*		products distributed with ANY DIGPAK drivers. 												 */
/*																																					 */
/*		To pay a license, simply write a check for $500 payable to						 */
/*				The Audio Solution, 747 Napa Lane, St. Charles, MO 63304					 */
/*				with a copy of your commerical product.  You will receive a signed */
/*				license agreement from The Audio Solution shortly thereafter. 		 */
/*				This license fee applies specifically to the inclusion with your	 */
/*				distribution disk any of the DIGPAK drivers from The Audio Solution*/
/*				These drivers are copyrighted works, created by me, to enhance the */
/*				use of sound and music in DOS based commercial software.	The 		 */
/*				license fees collected are used to maintain the drivers and keep	 */
/*				the BBS running.	There is a seperate license fee for the use 		 */
/*				and distribution of MIDPAK drivers. 															 */
/*																																					 */
/*				See accompaning documentation regarding license fees for MIDPAK 	 */
/*				distribution.  You would be ill-advised to distribute a commercial */
/*				product containing either DIGPAK and/or MIDPAK drivers without		 */
/*				having paid the distribution license fee.  Since your product would*/
/*				contain unlicensed copyrighted software from The Audio Solution,	 */
/*				your product could be required to be immediately removed from retail*/
/*				distribution.  I doubt this is going to be a problem.  Clearly if  */
/*				your product is enhanced by the use of these drivers, your company */
/*				can easily afford a nominal license fee of $500 in exchange for 	 */
/*				getting the use of several man-years of software engineering			 */
/*				resources.																												 */
/*****************************************************************************/

/* Bit flags to denote audio driver capabilities. */
/* returned by the AudioCapabilities call.				*/
#define PLAYBACK 1    // Bit zero true if can play audio in the background.
#define MASSAGE  2    // Bit one is true if data is massaged.
#define FIXEDFREQ 4   // Bit two is true if driver plays at fixed frequency.
#define USESTIMER 8   // Bit three is true, if driver uses timer.
#define SHARESTIMER 16 // Bit 4, timer can be shared
#define STEREOPAN 64	// Bit 6, supports stereo panning.
#define STEREOPLAY 128 // Bit 7, supports 8 bit PCM stereo playback.
#define AUDIORECORD 256 // Bit 8, supports audio recording!
#define DMABACKFILL 512  // Bit 9, support DMA backfilling.
#define PCM16 1024			// Bit 10, supports 16 bit digital audio.
#define PCM16STEREO 2048 // Bit 11, driver support 16 bit digital sound
// All digpak drivers which support 16 bit digital sound ASSUME 16 bit
// SIGNED data, whereas all 8 bit data is assumed UNSIGNED.  This correlates
// exactly to the hardware specifcations for most all DMA driven PC sound
// cards, including SB16, Gravis UltraSound and ProAudio Spectrum 16 sound cards.

typedef struct
{
	char far *sound;		 // address of audio data. IMPORANT SEE NOTE BELOW!!!!!!!!!!
	unsigned short sndlen; // Length of audio sample.
	short far *IsPlaying; // Address of play status flag.
	short frequency;	// Playback frequency. recommended 11khz.
} SNDSTRUC;

/**  !!! IMPORTANT NOT REGARDING THE SOUND ENTRY IN THE SOUND STRUCTURE !!! **/
/**  Since this sound structure is in the identical format as that used in	**/
/**  real-mode, this entry is NOT a flat-model 32 bit address but in real 	**/
/**  mode offset:segment format, in the first 1mb of address space.  The		**/
/**  sound effect cannot be greater than 64k in length! 										**/
/**  Use the DOSCALLS function call RealPtr to convert the 32 bit flat			**/
/**  address into offset:segment format.	Example: snd->sound = RealPtr(data)*/
/**  which will convert the low 1mb flat addres 'data' into offset:segment   */
/**  format and store it into the 'sound' entry of the SNDSTRUC structure    */
/**  pointed to by 'snd'.  'snd' MUST be in the first 1mb of address space as*/
/**  well.	'snd' is passed as a flat-model address, and DIGPAK 3.4 drivers  */
/**  will treat it as such so long as the SetDMPIMode has been set on.	This */
/**  is done automatically by LOADER.C during the InitDigPak call.					 */

extern short	cdecl far DigPlay(SNDSTRUC far *sndplay); // 688h -> Play 8 bit digitized sound.
extern short	cdecl far SoundStatus(void);	// 689h -> Report sound driver status.
extern void cdecl far MassageAudio(SNDSTRUC far *sndplay); // 68Ah -> Preformat 8 bit digitized sound.
extern void cdecl far DigPlay2(SNDSTRUC far *sndplay); // 68Bh -> Play preformatted data.
extern short	cdecl far AudioCapabilities(void); // 68Ch -> Report audio driver capabilities.
extern void cdecl far StopSound(void); // 68Fh -> Stop current sound from playing.
extern short	cdecl far PostAudioPending(SNDSTRUC far *sndplay);

#define NOTPLAYING 0  // No sound is playing.
#define PLAYINGNOTPENDING 1 // Playing a sound, but no sound is pending.
#define PENDINGSOUND 2			// Playing, and a sound is pending.
extern short	cdecl far AudioPendingStatus(void);

#define FULLRIGHT 0
#define FULLLEFT 127
#define FRONTANDCENTER 64
extern short	cdecl far SetStereoPan(short panvalue); // 0-127, 0 full right.
																//			 64 full volume both.
																//			127 full volume left.


#define PCM_8_MONO 0
#define PCM_8_STEREO 1
#define PCM_16_MONO 2
#define PCM_16_STEREO 3
extern short cdecl far SetPlayMode(short playmode);
// Return 0 if mode not available, 1 if mode set.

extern short cdecl far SetRecordMode(short mode); // Set audio recording mode.

extern short far * cdecl far PendingAddress(void); // Reports the far address of the pending
// flag.	If this memory location pointed to by this address is 1 that means
// a pending sound effect is still pending.  When this becomes zero, then your
// application software can post the next piece of audio to play.  It is
// preferable to use this semaphore to know when to post the next buffer
// rather than to use the AudioPendingStatus call.

extern short far * cdecl far ReportSemaphoreAddress(void); // Reports the far address of the DigPak
// semaphore.  If this semaphore is true, then DigPak is currently active
// and you shouldn't post any DigPak calls.  This is EXTREMELY important if
// you are trying to invoke DigPak functions via a hardware interrupt, where
// you could potentially have interrupted DigPak itself.

extern short cdecl far ReportVersionNumber(void); // Report the DigPak version number.
// Return code is times 100, meaning that version 3.1 would be returned
// as the decimal number 310.  This function wasn't supported prior to
// version 3.1 release, so it will be returned as 0, for versions prior
// to 3.1.

extern short cdecl far SetBackFillMode(short mode); // Turn DMA backfill mode on/off, return code
// of 1 means mode was set.  Return code of 0 means driver doesn't support
// DMA backfill.

extern unsigned short cdecl far ReportDMAC(void); // Report current DMA counter.

extern short cdecl far VerifyDMA(char far *data,short length); // Verify this buffer block
// doesn't cross a 64k boundary.  Return code of 1 means the block is OK.
// return code of 0 means the block can't be used, try another.

extern void cdecl far NullSound(char far *sound,short sndlen,short null);

/* Support routines */
extern void cdecl far WaitSound(void); // Wait until sound playback completed.
extern short	cdecl far CheckIn(void);	 // Is sound driver available? 0 no, 1 yes.

extern void cdecl far SetDPMIMode(short mode); // Set DPMI addressing mode for DIGPAK

extern void cdecl far DigPakIdentityString(char *str);

extern short	cdecl far InitDP(short segment); // Bootstrap hardware.
extern void cdecl far DeInitDP(short segment); // Uninitialize DIGPAK
