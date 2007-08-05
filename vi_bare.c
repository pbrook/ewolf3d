#include "wl_def.h"

#define FIXME abort

#ifdef INTEGRATOR
/* Memory mapped peripherals.  */
#define clcdc	((volatile uint32_t *)0xc0000000)
#define kmi0	((volatile uint32_t *)0x18000000)
#endif

/* Input.  */

byte IN_MouseButtons()
{
    return 0;
}

void IN_GetMouseDelta(myint *dx, myint *dy)
{
}

void INL_Update()
{
#ifdef INTEGRATOR
    int scancode;
    while (kmi0[1] & 0x10) {
	int press = 1;
	scancode = kmi0[2];
#if 0
	printf ("scancode %x\n", scancode);
#else
	if (scancode == 0xe0) {
	    scancode = kmi0[2] | 0x100;
	}
	if ((scancode & 0xff) == 0xf0) {
	    press = 0;
	    scancode = (scancode & 0x100) | kmi0[2];
	}
	switch (scancode) {
	case 0x29: scancode = sc_Space; break;
	case 0x14: scancode = sc_Control; break;
	case 0x175: scancode = sc_UpArrow; break;
	case 0x172: scancode = sc_DownArrow; break;
	case 0x16b: scancode = sc_LeftArrow; break;
	case 0x174: scancode = sc_RightArrow; break;
	default: scancode = 0; break;
	}
	if (scancode == 0)
	    continue;
	keyboard_handler(scancode, press);
#endif
    }
#endif
}

/* Graphics bits.  */

byte *gfxbuf;
byte framebuffer[128 * 64];

void VL_Startup()
{
    gfxbuf = framebuffer;

#ifdef INTEGRATOR
    /* Initialize CLCDC.  */
    clcdc[0] = 0x1c; // Horizontal timing (128 pixels)
    clcdc[1] = 0x3f; // Vertical timing (64 rows)
    clcdc[4] = (uint32_t)framebuffer; // base address
    clcdc[7] = 0x827; // 8-bit TFT
#endif
}

void VL_Shutdown()
{
}

void VL_WaitVBL(int i)
{
}

void VW_UpdateScreen()
{
}

void VL_SetPalette(const byte *palette)
{
#ifdef INTEGRATOR
    int i;
    uint32_t val;
    const byte *p = palette;

    for (i = 0; i < 128; i++) {
	val = *(p++) >> 1;
	val |= (*(p++) & 0x3e) << 4;
	val |= (*(p++) & 0x3e) << 9;
	val |= (*(p++) & 0x3e) << 15;
	val |= (*(p++) & 0x3e) << 20;
	val |= (*(p++) & 0x3e) << 25;
	clcdc[i + 0x80] = val;
    }
#endif
}

void VL_GetPalette(byte *palette)
{
    FIXME();
}

void Quit(const char *error)
{
    FIXME();
}

void TimerInit();

int main(int argc, char **argv)
{
    vwidth = 128;
    vheight = 96;
    TimerInit();
    return WolfMain(argc, argv);
}


