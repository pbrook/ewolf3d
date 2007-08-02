#ifndef __ID_CA_H__
#define __ID_CA_H__

/* ======================================================================== */

#define NUMMAPS		60
#define MAPPLANES	2

typedef	struct
{
#ifdef ENABLE_PRECOMPILE
	myint planestart[2];
	umyshort planelength[2];
#else
	myint planestart[3];
	myint planelength[3];
	myint width, height;
#endif
} maptype;

/* ======================================================================== */

extern	myint	mapon;

extern	word	mapseg0[64 * 64];
extern	word	mapseg1[64 * 64];
#ifdef ENABLE_PRECOMPILE
extern	const maptype	mapheaderseg[NUMMAPS];
extern const word RLEWtag;
#else
extern	maptype	*mapheaderseg[NUMMAPS];
#endif
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

typedef word pool_id;

void MM_Startup();
void MM_Shutdown();

void MM_GetPtr(memptr *baseptr, unsigned long size);
memptr MM_AllocPool(pool_id *id, unsigned long size);
memptr MM_PoolPtr(pool_id id);
void MM_FreePtr(memptr *baseptr);

void MM_SetPurge(memptr *baseptr, myint purge);
void MM_SetLock(memptr *baseptr, boolean locked);
void MM_SortMem();

#define PMPageSize	4096

typedef	struct {
	umyshort offset;	/* Offset of chunk into file, >> 8 */
	myshort length;	/* Length of the chunk */
} PageListStruct;

#ifdef ENABLE_PRECOMPILE
extern const myint ChunksInFile, PMSpriteStart, PMSoundStart;
extern const PageListStruct PMPages[];
// FIXME: Could be 16-bit offsets.
extern memptr PageAddr[];
#else
extern myint ChunksInFile, PMSpriteStart, PMSoundStart;
extern PageListStruct *PMPages;
extern memptr *PageAddr;
#endif

#define	PM_GetSoundPage(v)	PM_GetPage(PMSoundStart + (v))
#define	PM_GetSpritePage(v)	PM_GetPage(PMSpriteStart + (v))
memptr PM_GetPage(myint pagenum);

void PM_Startup();
void PM_Shutdown();

#endif
