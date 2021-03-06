#include "wl_def.h"

#include <vga.h>
#include <vgakeyboard.h>

byte *gfxbuf = NULL;
byte *graphmem = NULL;

void keyboard_handler(myint code, myint press);
void keyboard_handlerx(myint code, myint press);

void DisplayTextSplash(byte *text);

/*
==========================
=
= Quit
=
==========================
*/

void Quit(const char *error)
{
	memptr screen = NULL;

	if (!error || !*error) {
		CA_CacheGrChunk(ORDERSCREEN);
		screen = grsegs[ORDERSCREEN];
		WriteConfig();
	} else if (error) {
		CA_CacheGrChunk(ERRORSCREEN);
		screen = grsegs[ERRORSCREEN];
	}
	
	ShutdownId();
	
	if (screen) {
		/* doesn't look too good on console at the moment ...*/
		/* DisplayTextSplash(screen); */
	}
	
	if (error && *error) {
		fprintf(stderr, "Quit: %s\n", error);
		exit(EXIT_FAILURE);
 	}
	exit(EXIT_SUCCESS);
}

void VL_WaitVBL(myint vbls)
{
	myint i;

	for (i = 0; i < vbls; i++)
		vga_waitretrace();                
}

void VW_UpdateScreen()
{
	VL_WaitVBL(1); 
	memcpy(graphmem, gfxbuf, vwidth*vheight);
}

/*
=======================
=
= VL_Startup
=
=======================
*/

void VL_Startup()
{
	myint mode;
	
	vga_init(); /* TODO: maybe move into main or such? */
	
	if (MS_CheckParm("x2")) {
		mode = G640x400x256;
		vwidth = 640;
		vheight = 400;
	} else {
		mode = G320x200x256;
		vwidth = 320;
		vheight = 200;
	}

	if (gfxbuf == NULL) 
		gfxbuf = malloc(vwidth * vheight * 1);
		
	if (vga_hasmode(mode) == 0) 
		Quit("vga_hasmode failed!");
			
	if (vga_setmode(mode) != 0)
		Quit("vga_setmode failed!");
		
	if ((mode != G320x200x256) && (vga_setlinearaddressing() == -1))
		Quit("vga_setlinearaddressing failed!");
		
 	graphmem = vga_getgraphmem();
 	
 	keyboard_init();
	keyboard_seteventhandler(keyboard_handlerx);
}

/*
=======================
=
= VL_Shutdown
=
=======================
*/

void VL_Shutdown()
{
	keyboard_close();
	
	if (gfxbuf != NULL) {
		free(gfxbuf);
		gfxbuf = NULL;
	}
	vga_setmode(TEXT);
}

/* ======================================================================== */

/*
=================
=
= VL_SetPalette
=
=================
*/

void VL_SetPalette(const byte *palette)
{
	myint i;
	
	VL_WaitVBL(1);
	
	for (i = 0; i < 256; i++)
		vga_setpalette(i, palette[i*3+0], palette[i*3+1], palette[i*3+2]);
}

/*
=================
=
= VL_GetPalette
=
=================
*/

void VL_GetPalette(byte *palette)
{
	myint i, r, g, b;
	
	for (i = 0; i < 256; i++) {
		vga_getpalette(i, &r, &g, &b);
		palette[i*3+0] = r;
		palette[i*3+1] = g;
		palette[i*3+2] = b;
	}
}

/*
=============================================================================

					GLOBAL VARIABLES

=============================================================================
*/

static myint SVGALibToScancode(myint key)
{
	switch(key) {
		case SCANCODE_BREAK:
		case SCANCODE_BREAK_ALTERNATIVE:
			return 0xe1; /* paused */
		case SCANCODE_CURSORBLOCKUP:
			return sc_UpArrow;
		case SCANCODE_CURSORBLOCKDOWN:
			return sc_DownArrow;
		case SCANCODE_CURSORBLOCKLEFT:
			return sc_LeftArrow;
		case SCANCODE_CURSORBLOCKRIGHT:
			return sc_RightArrow;
		case SCANCODE_HOME:
			return sc_Home;
		case SCANCODE_END:
			return sc_End;
		case SCANCODE_PAGEUP:
			return sc_PgUp;
		case SCANCODE_PAGEDOWN:
			return sc_PgDn;
		case SCANCODE_INSERT:
			return sc_Insert;
		case SCANCODE_REMOVE:
			return sc_Delete;
		default: /* rest should be the same hopefully */
			return key;
	}
}

void keyboard_handlerx(myint code, myint press)
{
	keyboard_handler(SVGALibToScancode(code), press);
}

byte IN_MouseButtons()
{
	return 0;
}

void IN_GetMouseDelta(myint *dx, myint *dy)
{
}

/*
===================
=
= IN_JoyButtons
=
===================
*/

byte IN_JoyButtons()
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	IN_GetJoyAbs() - Reads the absolute position of the specified joystick
//
///////////////////////////////////////////////////////////////////////////
void IN_GetJoyAbs(word joy,word *xp,word *yp)
{
	*xp = 0;
	*yp = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyDelta() - Returns the relative movement of the specified
//		joystick (from +/-127)
//
///////////////////////////////////////////////////////////////////////////
void INL_GetJoyDelta(word joy,myint *dx,myint *dy)
{
	*dx = 0;
	*dy = 0;
}

///////////////////////////////////////////////////////////////////////////
//
//	INL_GetJoyButtons() - Returns the button status of the specified
//		joystick
//
///////////////////////////////////////////////////////////////////////////
word INL_GetJoyButtons(word joy)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////
//
//      IN_SetupJoy() - Sets up thresholding values and calls INL_SetJoyScale()
//              to set up scaling values
//
///////////////////////////////////////////////////////////////////////////
void IN_SetupJoy(word joy,word minx,word maxx,word miny,word maxy)
{
}

void INL_Update()
{
	while (keyboard_update()) ; /* get all events */
}

myint main(myint argc, char *argv[])
{
	vwidth = 320;
	vheight = 200;

	return WolfMain(argc, argv);
}
