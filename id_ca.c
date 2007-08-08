#include "wl_def.h"

#ifndef ENABLE_PRECOMPILE
typedef struct
{
	byte val[2];
	boolean flag0:1;
	boolean flag1:1;
} huffnode;
#else
#include "huffman.h"
#endif
/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

#ifndef ENABLE_PRECOMPILE
static word RLEWtag;
maptype	*mapheaderseg[NUMMAPS];
#endif

myint mapon;

ms0	mapseg0[MAPSIZE * MAPSIZE];
#ifdef ENABLE_AUDIO
static byte	*audiosegs[NUMSNDCHUNKS];
#endif
static pool_id grsegs[NUMCHUNKS];

char extension[5];
#define gfilename "vgagraph."
#ifndef ENABLE_PRECOMPILE
#define gdictname "vgadict."
#define mheadname "maphead."
#define gheadname "vgahead."
#endif
#define gmapsname "gamemaps."
#define aheadname "audiohed."
#define afilename "audiot."
#define pfilename "vswap."

#ifdef ENABLE_AUDIO
static int32_t *audiostarts; /* array of offsets in audiot */
#endif

#ifndef ENABLE_PRECOMPILE
static int32_t grstarts[NUMCHUNKS + 1];	/* array of offsets in vgagraph */
static huffnode grhuffman[256];
#endif

static myint grhandle = -1;	/* handle to VGAGRAPH */
static myint maphandle = -1;	/* handle to GAMEMAPS */
#ifdef ENABLE_AUDIO
static myint audiohandle = -1;	/* handle to AUDIOT */
#endif

/*
=============================================================================

					   LOW LEVEL ROUTINES

=============================================================================
*/

static void CA_CannotOpen(const char *string)
{
	char str[30];

	strcpy(str, "Can't open ");
	strcat(str, string);
	strcat(str, "!\n");
	Quit(str);
}

#ifdef ENABLE_DEMO
/*
==========================
=
= CA_WriteFile
=
= Writes a file from a memory buffer
=
==========================
*/

boolean CA_WriteFile(const char *filename, const void *ptr, long length)
{
	ssize_t l;
	myint handle;

	handle = OpenWrite(filename);

	if (handle == -1)
		return false;

	l = WriteBytes(handle, (const byte *)ptr, length);
	if (l == -1) {
		perror("CA_FarWrite");
		return false;
	} else if (l == 0) {
		fprintf(stderr, "CA_FarWrite hit EOF?\n");
		return false;
	} else if (l != length) {
		fprintf(stderr, "CA_FarWrite only wrote %d out of %ld\n", l, length);
		return false;
	}

	CloseWrite(handle);
	
	return true;
}

/*
==========================
=
= CA_LoadFile
=
= Allocate space for and load a file
=
==========================
*/

boolean CA_LoadFile(const char *filename, memptr *ptr)
{
	myint handle;
	ssize_t l;
	long size;

	if ((handle = OpenRead(filename)) == -1)
		return false;

	size = ReadLength(handle);
	MM_GetPtr(ptr, size);
	
	l = ReadBytes(handle, (byte *)(*ptr), size);
	
	if (l == -1) {
		perror("CA_FarRead");
		return false;
	} else if (l == 0) { 
		fprintf(stderr, "CA_FarRead hit EOF?\n");
		return false;
	} else if (l != size) {
		fprintf(stderr, "CA_FarRead only read %d out of %ld\n", l, size);
		return false;
	}
	
	CloseRead(handle);
	
	return true;
}
#endif

/*
============================================================================

		COMPRESSION routines

============================================================================
*/

/*
======================
=
= CAL_HuffExpand
= Length is the length of the EXPANDED data
=
======================
*/

/* From Ryan C. Gordon -- ryan_gordon@hotmail.com */
void CAL_HuffExpand(const byte *source, byte *dest, long length, 
	const huffnode *htable)
{
	const huffnode *headptr;          
	const huffnode *nodeon;           
	byte      mask = 0x01;    
	word      path;             
	byte     *endoff = dest + length;    

	nodeon = headptr = htable + 254;

	do {
		if (*source & mask)
		  path = 1;
	        else
		  path = 0;
       		mask <<= 1;
	        if (mask == 0x00) {   
			mask = 0x01;
			source++;
	        } 
		if (path ? nodeon->flag1 : nodeon->flag0) {  
			nodeon = (htable + nodeon->val[path]);
		} else {
			*dest = nodeon->val[path];
			dest++;
			nodeon = headptr;
		}
	} while (dest != endoff);   
} 

/*
======================
=
= CAL_CarmackExpand
= Length is the length of the EXPANDED data
=
= Note: This function happens to implicity swap words' bytes around.
=       For maps, this happens to be the desired effect.
=
======================
*/

#define NEARTAG	0xa7
#define FARTAG	0xa8

void CAL_CarmackExpand(const byte *source, word *dest, word length)
{
	unsigned myint offset;
	word *copyptr, *outptr;	
	byte chhigh, chlow;
	const byte *inptr;
	
	length /= 2;

	inptr = source;
	outptr = dest;

	while (length) {		
		chlow = *inptr++; /* count */
		chhigh = *inptr++;
		
		if (chhigh == NEARTAG) {
			if (!chlow) {	
				/* have to insert a word containing the tag byte */
				*outptr++ = (chhigh << 8) | *inptr;
				inptr++;
				
				length--;
			} else {
				offset = *inptr;
				inptr++;
				
				copyptr = outptr - offset;
				
				length -= chlow;
				while (chlow--)
					*outptr++ = *copyptr++;
			}
		} else if (chhigh == FARTAG) {
			if (!chlow) {
				/* have to insert a word containing the tag byte */
				*outptr++ = (chhigh << 8) | *inptr;
				inptr++;
				
				length--;
			} else {
				offset = *inptr | (*(inptr+1) << 8);
				inptr += 2;
				
				copyptr = dest + offset;
				length -= chlow;
				while (chlow--)
					*outptr++ = *copyptr++;
			}
		} else {
			*outptr++ = (chhigh << 8) | chlow;
			length--;
		}
	}
}

/*
======================
=
= CA_RLEWexpand
=
======================
*/

void CA_RLEWexpand(const word *source, byte *dest, int offset)
{
	word value, count, i;
	word rlewtag = RLEWtag;
	byte *end = dest + 64 * 64;
	
	/* expand it */
	do {
		value = *source++;

		if (value != rlewtag) {
			/* uncompressed */
			*dest++ = value - offset;
		} else {
			/* compressed string */
			count = *source++;
			
			value = *source++ - offset;
			for (i = 0; i < count; i++)
				*dest++ = value;
		}
	} while (dest < end);
}

/*
=============================================================================

					 CACHE MANAGER ROUTINES

=============================================================================
*/

/*
======================
=
= CAL_SetupGrFile
=
======================
*/

/* TODO: build huffman table into flash.  */
static void CAL_SetupGrFile()
{
	char fname[13];
#ifndef ENABLE_PRECOMPILE
	byte *grtemp;
	myint i;

	myint handle;
/* load vgadict.ext (huffman dictionary for graphics files) */
	strcpy(fname, gdictname);
	strcat(fname, extension);

	handle = OpenRead(fname);
	if (handle == -1)
		CA_CannotOpen(fname);

	for (i = 0; i < 256; i++) {
	/* 0-255 is a character, > is a pointer to a node */
		myshort v;
		v = ReadInt16(handle);
		grhuffman[i].flag0 = v >> 8;
		grhuffman[i].val[0] = v & 0xff;
		v = ReadInt16(handle);
		grhuffman[i].flag1 = v >> 8;
		grhuffman[i].val[1] = v & 0xff;
	}
	
	CloseRead(handle);
	
/* load the data offsets from vgahead.ext */
	MM_GetPtr((memptr)&grtemp, (NUMCHUNKS+1)*3);
	
	strcpy(fname, gheadname);
	strcat(fname, extension);

	handle = OpenRead(fname);
	if (handle == -1)
		CA_CannotOpen(fname);
	
	ReadBytes(handle, grtemp, (NUMCHUNKS+1)*3);

	for (i = 0; i < NUMCHUNKS+1; i++)
		grstarts[i] = (grtemp[i*3+0]<<0)|(grtemp[i*3+1]<<8)|(grtemp[i*3+2]<<16);

	MM_FreePtr((memptr)&grtemp);
	
	CloseRead(handle);
#endif
	
/* Open the graphics file, leaving it open until the game is finished */
	strcpy(fname, gfilename);
	strcat(fname, extension);

	grhandle = OpenRead(fname);
	if (grhandle == -1)
		CA_CannotOpen(fname);

#ifndef ENABLE_PRECOMPILE
/* load the pic headers into pictable */
	CA_CacheGrChunk(STRUCTPIC);
	
	grtemp = CA_GetChunk(STRUCTPIC);
	for (i = 0; i < NUMPICS; i++) {
		pictable[i].width = grtemp[i*4+0] | (grtemp[i*4+1] << 8);
		pictable[i].height = grtemp[i*4+2] | (grtemp[i*4+3] << 8);
	}
	
	CA_UnCacheGrChunk(STRUCTPIC);
#endif
}

/* ======================================================================== */

/*
======================
=
= CAL_SetupMapFile
=
======================
*/

static void CAL_SetupMapFile()
{
#ifndef ENABLE_PRECOMPILE
	myint i;
	myint handle;
	long pos;
#endif
	char fname[13];
	
/* open the data file */
	strcpy(fname, gmapsname);
	strcat(fname, extension);

	maphandle = OpenRead(fname);
	if (maphandle == -1)
		CA_CannotOpen(fname);

#ifndef ENABLE_PRECOMPILE
	strcpy(fname, mheadname);
	strcat(fname, extension);

	handle = OpenRead(fname);
	if (handle == -1)
		CA_CannotOpen(fname);

	RLEWtag = ReadInt16(handle);

/* load all map header */
	for (i = 0; i < NUMMAPS; i++)
	{
		pos = ReadInt32(handle);
		if (pos == 0) {
			mapheaderseg[i] = NULL;
			continue;
		}
			
		MM_GetPtr((memptr)&mapheaderseg[i], sizeof(maptype));
		MM_SetLock((memptr)&mapheaderseg[i], true);

		ReadSeek(maphandle, pos, SEEK_SET);
		
		mapheaderseg[i]->planestart[0] = ReadInt32(maphandle);
		mapheaderseg[i]->planestart[1] = ReadInt32(maphandle);
		mapheaderseg[i]->planestart[2] = ReadInt32(maphandle);
		
		mapheaderseg[i]->planelength[0] = ReadInt16(maphandle);
		mapheaderseg[i]->planelength[1] = ReadInt16(maphandle);
		mapheaderseg[i]->planelength[2] = ReadInt16(maphandle);
		mapheaderseg[i]->width = ReadInt16(maphandle);
		mapheaderseg[i]->height = ReadInt16(maphandle);
	}

	CloseRead(handle);
#endif
	
}


/* ======================================================================== */

/*
======================
=
= CAL_SetupAudioFile
=
======================
*/

static void CAL_SetupAudioFile()
{
#ifdef ENABLE_AUDIO
	myint handle;
	long length;
	char fname[13];
	myint i;
	
	strcpy(fname, aheadname);
	strcat(fname, extension);

	handle = OpenRead(fname);
	if (handle == -1)
		CA_CannotOpen(fname);
	
	length = ReadLength(handle);
	
	MM_GetPtr((memptr)&audiostarts, length);
	
	for (i = 0; i < (length/4); i++)
		audiostarts[i] = ReadInt32(handle);

	CloseRead(handle);	

/* open the data file */

	strcpy(fname, afilename);
	strcat(fname, extension);

	audiohandle = OpenRead(fname);
	if (audiohandle == -1)
		CA_CannotOpen(fname);
#endif
}

/* ======================================================================== */

/*
======================
=
= CA_Startup
=
= Open all files and load in headers
=
======================
*/

void CA_Startup()
{
	CAL_SetupMapFile();
	CAL_SetupGrFile();
	CAL_SetupAudioFile();

	mapon = -1;
}

/*
======================
=
= CA_Shutdown
=
= Closes all files
=
======================
*/

void CA_Shutdown()
{
	CloseRead(maphandle);
	CloseRead(grhandle);
#ifdef ENABLE_AUDIO
	CloseRead(audiohandle);
#endif
}

/* ======================================================================== */

/*
======================
=
= CA_CacheAudioChunk
=
======================
*/

#ifdef ENABLE_AUDIO
void CA_CacheAudioChunk(myint chunk)
{
	myint pos, length;

	if (audiosegs[chunk])
		return;	

	pos = audiostarts[chunk];
	length = audiostarts[chunk+1]-pos;

	ReadSeek(audiohandle, pos, SEEK_SET);

	MM_GetPtr((memptr)&audiosegs[chunk], length);

	ReadBytes(audiohandle, audiosegs[chunk], length);
}

void CA_UnCacheAudioChunk(myint chunk)
{
	if (audiosegs[chunk] == 0) {
		fprintf(stderr, "Trying to free null audio chunk %d!\n", chunk);
		return;
	}
	
	MM_FreePtr((memptr *)&audiosegs[chunk]);
	audiosegs[chunk] = 0;
}
#endif

/*
======================
=
= CA_LoadAllSounds
=
======================
*/

void CA_LoadAllSounds()
{
#ifdef ENABLE_AUDIO
	myint start, i;

	for (start = STARTADLIBSOUNDS, i = 0; i < NUMSOUNDS; i++, start++)
		CA_CacheAudioChunk(start);
#endif
}

/* ======================================================================== */

/*
======================
=
= CAL_ExpandGrChunk
=
= Does whatever is needed with a pointer to a compressed chunk
=
======================
*/

static byte *CAL_ExpandGrChunk(myint chunk, const byte *source)
{
	myint tilecount = 0, i;
	long expanded;
	byte *dest;
	
	myint width = 0, height = 0;
	
	if (chunk >= STARTTILE8 && chunk < STARTEXTERNS)
	{
	/* expanded sizes of tile8 are implicit */
		expanded = 8*8*NUMTILE8;
		width = 8;
		height = 8;
		tilecount = NUMTILE8;
	} else if (chunk >= STARTPICS && chunk < STARTTILE8) {
		width = pictable[chunk - STARTPICS].width;
		height = pictable[chunk - STARTPICS].height;
		expanded = source[0]|(source[1]<<8)|(source[2]<<16)|(source[3]<<24);
		source += 4;
	} else {
	/* everything else has an explicit size longword */
		expanded = source[0]|(source[1]<<8)|(source[2]<<16)|(source[3]<<24);
		source += 4;
	}

/* allocate final space and decompress it */
	dest = MM_AllocPool(&grsegs[chunk], expanded);
	CAL_HuffExpand(source, dest, expanded, grhuffman);
	if (width && height) {
		if (tilecount) {
			for (i = 0; i < tilecount; i++) 
				VL_DeModeXize(dest+(width*height)*i, width, height);
		} else			
			VL_DeModeXize(dest, width, height);
	}
	return dest;
}

/*
======================
=
= CA_GetChunk
=
= Makes sure a given chunk is in memory, loadiing it if needed
=
======================
*/

memptr CA_GetChunk(myint chunk)
{
	long pos, compressed;
	byte *source;
	byte *dest;

	if (grhandle == -1)
		return NULL;
		
	if (grsegs[chunk]) {
		return MM_PoolPtr(grsegs[chunk]);
	}

/* load the chunk into a buffer */
	pos = grstarts[chunk];

	compressed = grstarts[chunk+1]-pos;

	ReadSeek(grhandle, pos, SEEK_SET);

	source = MM_AllocPool(NULL, compressed);
	//MM_GetPtr((memptr)&source, compressed);
	ReadBytes(grhandle, source, compressed);

	dest = CAL_ExpandGrChunk(chunk, source);
	
	//MM_FreePtr((memptr)&source);
	return dest;
}

void CA_UnCacheGrChunk(myint chunk)
{
}
void CA_CacheGrChunk(myint chunk)
{
}
	
/* ======================================================================== */

/*
======================
=
= CA_CacheMap
=
======================
*/

void CA_CacheMap(myint mapnum, myint plane)
{
	long pos,compressed;
	byte *source;
	memptr buffer2seg;
	long expanded;
	
	mapon = mapnum;

/* load plane into the already allocated buffers */

#ifdef ENABLE_PRECOMPILE
	pos = mapheaderseg[mapnum].planestart[plane];
	compressed = mapheaderseg[mapnum].planelength[plane];
#else
	pos = mapheaderseg[mapnum]->planestart[plane];
	compressed = mapheaderseg[mapnum]->planelength[plane];
#endif

	ReadSeek(maphandle, pos, SEEK_SET);
	
	source = MM_AllocPool(NULL, compressed);
	//MM_GetPtr((void *)&source, compressed);

	ReadBytes(maphandle, (byte *)source, compressed);
	
	expanded = source[0] | (source[1] << 8);		
	buffer2seg = MM_AllocPool(NULL, expanded);
	//MM_GetPtr(&buffer2seg, expanded);

/* NOTE: CarmackExpand implicitly fixes endianness, a RLEW'd only map
 would (likely) need to be swapped in CA_RLEWexpand
 
 Wolfenstein 3D/Spear of Destiny maps are always Carmack'd so this
 case is OK.  CA_RLEWexpand would need to be adjusted for Blake Stone
 and the like.
*/         		
	CAL_CarmackExpand(source+2, (word *)buffer2seg, expanded);
	//MM_FreePtr((void *)&source);

	CA_RLEWexpand(((word *)buffer2seg)+1, mapseg0, plane ? 16 : 0);
	//MM_FreePtr(&buffer2seg);
}

/* ======================================================================== */

#define POOL_SIZE 26*1024
static byte MM_Pool[POOL_SIZE];
unsigned int pool_offset;

typedef struct
{
  /* FIXME: This is broken on 64-bit hosts.  */
  pool_id *owner;
  uint16_t size;
  uint16_t pad;
} pool_header;

void MM_Startup()
{
  pool_header *p;
  p = (pool_header *)MM_Pool;
  p->owner = NULL;
  p->size = POOL_SIZE - 8;
  pool_offset = 0;
}

void MM_Shutdown()
{
}

memptr MM_AllocPool(pool_id *id, unsigned long size)
{
    pool_header *h;
    pool_header *next;
    h = (pool_header *)(MM_Pool + pool_offset);
    size = (size + 7) & ~7;
    if (size > POOL_SIZE - 8)
	Quit("Pool allocation too big\n");
    /* Reclaim entries until we have enough space.  */
    while (h->size < size)
      {
	/* Wrap back to the start of the pool.  */
	if (pool_offset + h->size + 8 == POOL_SIZE)
	  {
	    pool_offset = 0;
	    h = (pool_header *)MM_Pool;
	    continue;
	  }
	next = (pool_header *)(MM_Pool + pool_offset + h->size + 8);
	if (next->owner)
	  *next->owner = 0;
	h->size += next->size + 8;
      }
    /* Make sure this entry is free.  */
    if (h->owner)
      {
	*h->owner = 0;
      }
    if (size < h->size - 8) {
	next = (pool_header *)(MM_Pool + pool_offset + size + 8);
	next->owner = NULL;
	next->size = h->size - (size + 8);
	h->size = size;
    }
    h->owner = id;
    if (id)
	*id = pool_offset + 8;
    pool_offset += h->size + 8;
    if (pool_offset == POOL_SIZE)
      pool_offset = 0;
    return (memptr)(h + 1);
}

memptr MM_PoolPtr(pool_id id)
{
    if (!id)
	Quit("Pool entry absent\n");
    return (memptr)(MM_Pool + id);
}

#ifndef EMBEDDED
static int total_size = 0;
static int lastmalloc;
void MM_GetPtr(memptr *baseptr, unsigned long size)
{
	/* add some sort of linked list for purging */
  total_size += size;
  lastmalloc = size;
  //printf ("malloc %8d/%d\n", (int)size, total_size);
	*baseptr = malloc(size);
}

void MM_FreePtr(memptr *baseptr)
{
	/* add some sort of linked list for purging, etc */
	free(*baseptr);
}

void MM_SetPurge(memptr *baseptr, myint purge)
{
}

void MM_SetLock(memptr *baseptr, boolean locked)
{
#ifdef ENABLE_PRECOMPILE
  printf ("Locked Page\n");
  abort();
#else
  static int total_locked = 0;
  total_locked += lastmalloc;
#endif
}
#endif

void MM_SortMem()
{
}

static boolean PMStarted;

static myint PageFile = -1;
#ifndef ENABLE_PRECOMPILE
myint ChunksInFile, PMSpriteStart, PMSoundStart;
PageListStruct *PMPages;
#endif

static void PML_ReadFromFile(byte *buf, long offset, word length)
{
	if (!buf)
		Quit("PML_ReadFromFile: Null pointer");
	if (!offset)
		Quit("PML_ReadFromFile: Zero offset");
	if (ReadSeek(PageFile, offset, SEEK_SET) != offset)
		Quit("PML_ReadFromFile: Seek failed");
	if (ReadBytes(PageFile, buf, length) != length)
		Quit("PML_ReadFromFile: Read failed");
}

static void PML_OpenPageFile()
{
	char fname[13];
#ifndef ENABLE_PRECOMPILE
	myint i;
	PageListStruct *page;
	int offset;
#endif
	
	strcpy(fname, pfilename);
	strcat(fname, extension);
	
	PageFile = OpenRead(fname);
	if (PageFile == -1)
		Quit("PML_OpenPageFile: Unable to open page file");

#ifndef ENABLE_PRECOMPILE
	/* Read in header variables */
	ChunksInFile = ReadInt16(PageFile);
	PMSpriteStart = ReadInt16(PageFile);
	PMSoundStart = ReadInt16(PageFile);

	/* Allocate and clear the page list */
	MM_GetPtr((memptr)&PMPages, sizeof(PageListStruct) * ChunksInFile);
	MM_SetLock((memptr)&PMPages, true);
	
	memset(PMPages, 0, sizeof(PageListStruct) * ChunksInFile);

	/* Read in the chunk offsets */
	for (i = 0, page = PMPages; i < ChunksInFile; i++, page++) {
		offset = ReadInt32(PageFile);
		if (offset & 0xff0000ff)
		  {
		    printf ("Bad page offset 0x%x\n", offset);
		    exit(1);
		  }
		page->offset = offset >> 8;
	}
		
	/* Read in the chunk lengths */
	for (i = 0, page = PMPages; i < ChunksInFile; i++, page++) {	
		page->length = ReadInt16(PageFile);
	}

	MM_GetPtr((memptr)&PageAddr, sizeof(pool_id) * ChunksInFile);
	MM_SetLock((memptr)&PageAddr, true);
	memset(PageAddr, 0, sizeof(pool_id) * ChunksInFile);
#endif
}

static void PML_ClosePageFile()
{
	if (PageFile != -1)
		CloseRead(PageFile);
		
#ifndef ENABLE_PRECOMPILE
	if (PMPages) {
		myint i;
		
		for (i = 0; i < ChunksInFile; i++) {
			PageListStruct *page;
			
			page = &PMPages[i];
			if (page->addr != NULL) {
				MM_FreePtr((memptr)&page->addr);
			}
		}
		
		MM_SetLock((memptr)&PMPages,false);
		MM_FreePtr((memptr)&PMPages);
	}
#endif
}

memptr PM_GetPage(myint pagenum)
{
	memptr addr;
	if (pagenum >= ChunksInFile)
		Quit("PM_GetPage: Invalid page request");

#ifdef EMBEDDED
	if (RomChunks[pagenum]) {
	    return (memptr)RomChunks[pagenum];
	}
#endif
	if (PageAddr[pagenum]) {
		addr = MM_PoolPtr(PageAddr[pagenum]);
	} else {
		const PageListStruct *page = &PMPages[pagenum];
		addr = MM_AllocPool(&PageAddr[pagenum], page->length);
		PML_ReadFromFile(addr, (int32_t)page->offset << 8,
				 page->length);
	}
	return addr;
}

void PM_Startup()
{
	if (PMStarted)
		return;

	PML_OpenPageFile();

	PMStarted = true;
}

void PM_Shutdown()
{
	if (!PMStarted)
		return;

	PML_ClosePageFile();
}
