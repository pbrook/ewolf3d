#include "wl_def.h"

#define FIXME abort

#ifdef INTEGRATOR
/* Memory mapped peripherals.  */
#define clcdc	((volatile uint32_t *)0xc0000000)
#define kmi0	((volatile uint32_t *)0x18000000)
#endif

#ifdef LUMINARY
#define SYSCTL 0x400fe000
#define HWREG(x) (*((volatile uint32_t *)(x)))
#define GPIOA 0x40004000
#define GPIOB 0x40005000
#define GPIOC 0x40006000
#define GPIOD 0x40007000
#define GPIOE 0x40024000
#define GPIOF 0x40025000
#define GPIOG 0x40026000
#define SSI0  0x40008000

int read_buttons()
{
    int buttons;

    buttons = HWREG(GPIOE + 0x03c);
    if (HWREG(GPIOF + 0x008))
	buttons |= 0x10;
    /* Buttons are active low.  */
    buttons ^= 0x1f;
    return buttons;
}

#define OSRAM_INIT_REMAP    0x52
#define OSRAM_INIT_OFFSET   0x4C
//*****************************************************************************
//
// The sequence of commands used to initialize the SSD0323 controller.  Each
// command is described as follows:  there is a byte specifying the number of
// bytes in the command sequence, followed by that many bytes of command data.
// Note:  This initialization sequence is derived from OSRAM App Note AN018.
//
//*****************************************************************************
static const uint8_t oled_init_strings[] =
{
    //
    // Column Address
    //
    4, 0x15, 0, 63, 0xe3,

    //
    // Row Address
    //
    4, 0x75, 0, 63, 0xe3,

    //
    // Contrast Control
    //
    3, 0x81, 50, 0xe3,

    //
    // Half Current Range
    //
    2, 0x85, 0xe3,

    //
    // Display Re-map
    //
    3, 0xA0, OSRAM_INIT_REMAP, 0xe3,

    //
    // Display Start Line
    //
    3, 0xA1, 0, 0xe3,

    //
    // Display Offset
    //
    3, 0xA2, OSRAM_INIT_OFFSET, 0xe3,

    //
    // Display Mode Normal
    //
    2, 0xA4, 0xe3,

    //
    // Multiplex Ratio
    //
    3, 0xA8, 63, 0xe3,

    //
    // Phase Length
    //
    3, 0xB1, 0x22, 0xe3,

    //
    // Row Period
    //
    3, 0xB2, 70, 0xe3,

    //
    // Display Clock Divide
    //
    3, 0xB3, 0xF1, 0xe3,

    //
    // VSL
    //
    3, 0xBF, 0x0D, 0xe3,

    //
    // VCOMH
    //
    3, 0xBE, 0x02, 0xe3,

    //
    // VP
    //
    3, 0xBC, 0x10, 0xe3,

    //
    // Gamma
    //
    10, 0xB8, 0x01, 0x11, 0x22, 0x32, 0x43, 0x54, 0x65, 0x76, 0xe3,

    //
    // Set DC-DC
    3, 0xAD, 0x03, 0xe3,

    //
    // Display ON/OFF
    //
    2, 0xAF, 0xe3,

    // Done.
    0
};

static void ssi_flush_rx()
{
    while (HWREG(SSI0 + 0x00c) & 0x04)
	HWREG(SSI0 + 0x008);
}

static void oled_write(const uint8_t *p, int n, int command)
{
    int left;

    if (command)
	HWREG(GPIOC + 0x200) = 0;
    else
	HWREG(GPIOC + 0x200) = 0x80;

    /* Use the RX fifo to determine when the transfer completes.  */
    left = n;
    while (left) {
	uint32_t status;
	status = HWREG(SSI0 + 0x00c);
	/* Send data if TX FIFO not full.  */
	if (n == left && (status & 0x02)) {
	    HWREG(SSI0 + 0x008) = *(p++);
	    n--;
	}
	/* Pull data out of the RX fifo.  */
	if (status & 0x04) {
	    HWREG(SSI0 + 0x008);
	    left--;
	}
    }
}

static void oled_init()
{
    int i;
    const uint8_t *p;

    p = oled_init_strings;
    while (*p) {
	i = *(p++);
	oled_write(p, i, 1);
	p += i;
    }

}

void oled_render()
{
    uint8_t buf[16];
    int i;
    int j;
    byte *p;

    buf[0] = 0x15;
    buf[1] = 0;
    buf[2] = 63;
    oled_write(buf, 3, 1);
    buf[0] = 0x75;
    buf[1] = 0;
    buf[2] = 63;
    oled_write(buf, 3, 1);
    buf[0] = 0xA0;
    buf[1] = 0x52;
    oled_write(buf, 2, 1);

    p = gfxbuf;
    for (i = 0; i < 128 * 64 / 32; i++) {
	// FIXME: Translate and stream to OLED on the fly.
	// FIXME: Double buffer and send async.
	for (j = 0; j < 16; j++) {
	    buf[j] = pal4bit[*(p++)] << 4;
	    buf[j] |= pal4bit[*(p++)];
	}
	oled_write(buf, 16, 0);
    }
}

/* Turn off screen to prevernt burn-in.  */
void VL_ScreenSaver()
{
    static const byte offcmd[] = {0xAE, 0xAD, 0x02};
    oled_write(offcmd, 3, 1);
    while (read_buttons() == 0)
	asm volatile ("wfi");
    oled_init();
}
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
#ifdef LUMINARY
    int buttons = 0;
    static int oldbuttons = 0;
    int i;
    static const byte scancodes[5] = {sc_UpArrow, sc_DownArrow, sc_LeftArrow,
	sc_RightArrow, sc_Space};

    buttons = read_buttons();
    buttons ^= oldbuttons;
    oldbuttons ^= buttons;
    for (i = 0; i < 5; i++) {
	int mask = 1 << i;
	if (buttons & mask) {
	    keyboard_handler(scancodes[i], (oldbuttons & mask) != 0);
	}
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
#ifdef LUMINARY
    oled_render();
#endif
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

static void sys_init()
{
#ifdef LUMINARY
    /* Bump LDO voltage to workaround silicon bugs.  */
    HWREG(SYSCTL + 0x034) = 0x1b;
    /* Clock thePLL to 50MHz.  */
    HWREG(SYSCTL + 0x060) = 0x01d40b80;
    /* Wait for PLL to sync, then enable.  */
    while ((HWREG(SYSCTL + 0x050) & 0x40) == 0)
	/* no-op */ ;
    HWREG(SYSCTL + 0x060) &= ~0x0800;

    /* Enable peripherals.  */
    /* Timer0, SSI0.  */
    HWREG(SYSCTL + 0x104) |= 0x00010010;
    /* GPIO[A-G] */
    HWREG(SYSCTL + 0x108) |= 0x0000007f;

    /* Enable SSI output pins (GPIOA 2, 3, 5).  */
    HWREG(GPIOA + 0x420) |= 0x2c; /* HW fn. */
    HWREG(GPIOA + 0x400) |= 0x2c; /* Output.  */
    HWREG(GPIOA + 0x51c) |= 0x2c; /* Digital.  */
    HWREG(GPIOA + 0x508) |= 0x2c; /* 8mA drive strength.  */
    HWREG(GPIOA + 0x510) |= 0x2c; /* Pull-up.  */

    /* D/Cn output for OLED.  */
    HWREG(GPIOC + 0x400) |= 0x80; /* Output */
    HWREG(GPIOC + 0x51c) |= 0x80; /* Digital */
    HWREG(GPIOC + 0x508) |= 0x80; /* 8mA drive strength. */
    HWREG(GPIOC + 0x000) |= 0x80; /* Active.  */
    HWREG(GPIOC + 0x510) |= 0x80; /* Pull-up.  */

    /* Initialize SSI controller.  */
    HWREG(SSI0 + 0x004) = 0; /* Disable.  */
    /* 50MHz / 3.5Mbit = 14.2 cycles/bit.  */
    HWREG(SSI0 + 0x010) = 2; /* prescale /2 */
    HWREG(SSI0 + 0x000) = 0x0647; /* SCR=6 (+1), SPI polarity=1, 8bit */
    HWREG(SSI0 + 0x04) = 2; /* Enable.  */

    /* User pushbutton input.  */
    HWREG(GPIOF + 0x51c) |= 0x02; /* Digital */
    HWREG(GPIOF + 0x510) |= 0x02; /* Pull-up.  */

    /* Direction buttons.  */
    HWREG(GPIOE + 0x51c) |= 0x0f; /* Digital */
    HWREG(GPIOE + 0x510) |= 0x0f; /* Pull-up.  */

    ssi_flush_rx();

    oled_init();
#endif
}

int main(int argc, char **argv)
{
    vwidth = 128;
    vheight = 96;
    sys_init();
    TimerInit();
    return WolfMain(argc, argv);
}


