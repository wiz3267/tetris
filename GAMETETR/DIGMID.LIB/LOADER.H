// ** C prototype header to MIDPAK and DIGPAK loader functions.
int LoadDigPak(char far *fname); // Load digpak driver, returns' true if loaded.
int InitDigPak(void); // Initialize Digpak true, if successfull.
int DeInitDigPak(void); // Uninitialize DigPak.
int UnLoadDigPak(void); // unload digpak driver from memory.

int LoadMidPak(char far *fname,char far *adv,char far *ad); // Load MidPak driver, returns' true if loaded.
int InitMidPak(void); // Initialize MidPak true, if successfull.
int DeInitMidPak(void); // Uninitialize MidPak.
int UnLoadMidPak(void); // unload MidPak driver from memory.
