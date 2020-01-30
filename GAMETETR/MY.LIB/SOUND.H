//**************************************************************
//** описание класса 'SOUND' - проигрывание и обработка звука **
//**************************************************************
struct SNDSTRUCT
{
	unsigned char far * sound;	//дальний указатель на звуковые данные
	unsigned int sndlen;		//длина звуковой последовательности
	unsigned int far * IsPlaying;	//адрес флага состояния
	unsigned int frequency;		//частота воспроизведения
	int flag;			//как проиграть музыку
};

class SOUND
{
#define count_max 25	//это число можно менять: максимальное число
			//мелодий
#define NOSTOP	1	//звук не должна прерываться
#define YESSTOP 0	//звук может прерваться следующий звуком
#define YESPLAYING 1
#define NOPLAYING 0
private:
static int CountObject;
	int current;		//текущая мелодия
	int count;		//число звуков
	int load_status;	//статус загрузки
	char DIGPAK;		//флаг загрузки DIGPAK
	char MIDPAK;		//флаг загрузки MIDPAK
	char INITDIGPAK;	//флаг инициализации DIGPAK
	char INITMIDPAK;	//флаг инициализации MIDPAK
	char DIGPAKAVAILABLE;	//флаг доступности DIGPAK из под MIDPAK
	unsigned int isplay;	//флаг состояния
public:
	SNDSTRUCT sounds[count_max]; //массив указателей,на информацию о звуках
	SOUND(void);			//конструктор
	~SOUND(void);			//деструктор
	int  load(char far * fname,int=11025);	//прибавить мелодию
//	void set(int n, SNDSTRUCT far & sndplay); //установить свойства
	void play(int n,int flag=0);	//проиграть мелодию с флагом
	void SndPlay(SNDSTRUCT far & sndplay);	//проиграть звук
	int  soundStatus(void);		//состояние драйвера
	void stop(void);		//остановить проигрывание
	void unload(int n);		//удалить заданную мелодию с номером n
};