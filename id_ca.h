#ifndef __ID_CA_H__
#define __ID_CA_H__

/* ======================================================================== */

#define NUMMAPS		60
#define MAPPLANES	2

typedef	struct
{
	myint planestart[3];
	myint planelength[3];
	myint width, height;
	char name[16];
} maptype;

/* ======================================================================== */

extern	myint	mapon;

extern	word	*mapsegs[MAPPLANES];
extern	maptype	*mapheaderseg[NUMMAPS];
extern	byte	*audiosegs[NUMSNDCHUNKS];
extern	byte	*grsegs[NUMCHUNKS];

extern char extension[5];

/* ======================================================================== */

boolean CA_LoadFile(const char *filename, memptr *ptr);
boolean CA_WriteFile(const char *filename, const void *ptr, long length);

void CA_Startup();
void CA_Shutdown();

#ifdef ENABLE_AUDIO
void CA_CacheAudioChunk(myint chunk);
void CA_UnCacheAudioChunk(myint chunk);
#endif
void CA_LoadAllSounds();

void CA_CacheMap(myint mapnum);
void CA_CacheGrChunk(myint chunk);
void CA_UnCacheGrChunk(myint chunk);

/* ======================================================================= */

void MM_Startup();
void MM_Shutdown();

void MM_GetPtr(memptr *baseptr, unsigned long size);
void MM_FreePtr(memptr *baseptr);

void MM_SetPurge(memptr *baseptr, myint purge);
void MM_SetLock(memptr *baseptr, boolean locked);
void MM_SortMem();

#define PMPageSize	4096

typedef	struct {
	int32_t offset;	/* Offset of chunk into file */
	myshort length;	/* Length of the chunk */
	memptr addr;
} PageListStruct;

extern myint ChunksInFile, PMSpriteStart, PMSoundStart;

extern PageListStruct *PMPages;

#define	PM_GetSoundPage(v)	PM_GetPage(PMSoundStart + (v))
#define	PM_GetSpritePage(v)	PM_GetPage(PMSpriteStart + (v))
memptr PM_GetPage(myint pagenum);
#define	PM_FreeSoundPage(v)	PM_FreePage(PMSoundStart + (v))
#define	PM_FreeSpritePage(v)	PM_FreePage(PMSpriteStart + (v))
void PM_FreePage(myint pagenum);

void PM_Startup();
void PM_Shutdown();

#endif
