#include "wl_def.h"

#define INTEGRATOR 1

#define FIXME abort

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
    /* Read input here.  */
}

/* Graphics bits.  */

byte *gfxbuf;
byte framebuffer[128 * 96];

#define clcdc ((volatile uint32_t *)0xc0000000)

void VL_Startup()
{
    gfxbuf = framebuffer;

#ifdef INTEGRATOR
    /* Initialize CLCDC.  */
    clcdc[0] = 0x1c; // Horizontal timing (128 pixels)
    clcdc[1] = 0x95; // Vertical timing (96 rows)
    clcdc[4] = (uint32_t)framebuffer; // Vertical timing (96 rows)
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

int main(int argc, char **argv)
{
    vwidth = 128;
    vheight = 96;
    return WolfMain(argc, argv);
}


