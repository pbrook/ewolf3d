#include "wl_def.h"

#define FIXME abort

#ifdef INTEGRATOR
/* Memory mapped peripherals.  */
#define clcdc	((volatile uint32_t *)0xc0000000)
#define kmi0	((volatile uint32_t *)0x18000000)
#endif

#ifdef LUMINARY
#include "luminary.h"

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

/* Switch between OLED and SD card.  */
void ssi_select(int dev)
{
    static int curdev = 2;
    if (dev == curdev)
	return;
    curdev = dev;
    if (dev) {
	/* SD card.  */
	//HWREG(GPIOA + 0x504) |= 0x3c; /* 4mA drive strength.  */
	HWREG(SSI0 + 0x004) = 0; /* Disable.  */
	/* Force fss device select high.  */
	HWREG(GPIOA + 0x020) = 1;
	HWREG(GPIOA + 0x420) &= ~0x08;
	HWREG(SSI0 + 0x010) = 2; /* prescale /2 */
	/* Initial config is done at 400kHz, then switch to full speed.  */
	/* 50MHz / 400kbit = 125 cycles/bit.  */
	/* SCR=62 (+1), SPI polarity=0, 8bit */
	if (dev == 2)
	    HWREG(SSI0 + 0x000) = 0x3e07;
	else
	    HWREG(SSI0 + 0x000) = 0x0007;
	HWREG(SSI0 + 0x04) = 2; /* Enable.  */
    } else {
	/* OLED */
	HWREG(SSI0 + 0x004) = 0; /* Disable.  */
	/* Enable fss device select.  */
	HWREG(GPIOA + 0x420) |= 0x08;
	HWREG(GPIOA + 0x020) = 0;
	/* 50MHz / 3.5Mbit = 14.2 cycles/bit.  */
	HWREG(SSI0 + 0x010) = 2; /* prescale /2 */
	HWREG(SSI0 + 0x000) = 0x0647; /* SCR=6 (+1), SPI polarity=1, 8bit */
	HWREG(SSI0 + 0x04) = 2; /* Enable.  */
    }

}

static void ssi_flush_rx()
{
    while (HWREG(SSI0 + 0x00c) & 0x04)
	HWREG(SSI0 + 0x008);
}

static volatile byte *ssi_data;
static volatile int ssi_tosend;
static volatile int ssi_left;

void ssi_isr()
{
    uint32_t status;
    uint32_t mask;
    while (1) {
	status = HWREG(SSI0 + 0x00c);
	mask = ssi_tosend ? 0x06 : 0x04;
	if ((status & mask) == 0)
	  break;
	/* Send data if TX FIFO not full.  */
	if (ssi_tosend && (status & 0x02)) {
	    HWREG(SSI0 + 0x008) = *(ssi_data++);
	    ssi_tosend--;
	}
	/* Pull data out of the RX fifo.  */
	if (status & 0x04) {
	    HWREG(SSI0 + 0x008);
	    ssi_left--;
	}
    }
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

    ssi_select(0);
    p = oled_init_strings;
    while (*p) {
	i = *(p++);
	oled_write(p, i, 1);
	p += i;
    }

}

static void oled_setwindow()
{
    static byte cmd[6] = {0x15, 0, 63, 0x75, 0, /* height-1 */0};
    ssi_select(0);
    cmd[5] = oled_height - 1;
    oled_write(cmd, 6, 1);
}

void oled_clear()
{
    uint8_t buf[4];
    int i;
    oled_setwindow();
    memset(buf, 0, 4);
    for (i = 0; i < oled_height * (128 / 8); i++) {
	oled_write(buf, 4, 0);
    }
}

void oled_render()
{
    uint8_t buf[16];
#ifndef EMBEDDED
    int i;
    int j;
    byte *p;
#endif

    oled_setwindow();
    buf[0] = 0xA0;
    buf[1] = 0x52;
    oled_write(buf, 2, 1);


    ssi_data = gfxbuf;
    ssi_tosend = ssi_left = 128 * 64 / 2;
    HWREG(GPIOC + 0x200) = 0x80;
    /* Enable SSI interrupts.  */
    HWREG(SSI0 + 0x014) = 0xc;
    /* Wait for transfer to complete.  */
    asm volatile ("1:\n\t"
		  "cpsid i\n\t"
		  "ldr r0, [%0]\n\t"
		  "cmp r0, #0\n\t"
		  "it ne\n\t"
		  "wfine\n\t"
		  "cpsie i\n\t"
		  "bne 1b\n\t"
		  : : "r" (&ssi_tosend) : "r0");
    /* Mask SSI interrupts.  */
    HWREG(SSI0 + 0x014) = 0x0;
    /* Pull remaining bytes out of FIFO.  */
    while (ssi_left) {
	uint32_t status;
	status = HWREG(SSI0 + 0x00c);
	if (status & 0x04) {
	    HWREG(SSI0 + 0x008);
	    ssi_left--;
	}
    }
#if 0
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
#endif
}

/* Turn off screen to prevernt burn-in.  */
void VL_ScreenSaver()
{
    static const byte offcmd[] = {0xAE, 0xAD, 0x02};
    ssi_select(0);
    oled_write(offcmd, 3, 1);
    while (read_buttons() == 0)
	asm volatile ("wfi");
    oled_init();
}
#endif

/* Input.  */

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

byte framebuffer[128 * 64 / 2];
byte *gfxbuf = framebuffer;

void VL_Startup()
{
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

#ifdef INTEGRATOR
void VL_SetPalette(const byte *palette)
{
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
}

void VL_GetPalette(byte *palette)
{
    FIXME();
}
#endif

#ifdef DEBUG
void Quit(const char *error)
{
    FIXME();
}
#endif

void TimerInit();

static void sys_init()
{
#ifdef LUMINARY
    /* Enable peripherals.  */
    /* Timer0, SSI0.  */
    HWREG(SYSCTL + 0x104) |= 0x00010010;
    /* GPIO[A-G] */
    HWREG(SYSCTL + 0x108) |= 0x0000007f;

    /* Bump LDO voltage to workaround silicon bugs.  */
    //HWREG(SYSCTL + 0x034) = 0x1b;
    /* Clock the PLL to 50MHz.  */
    HWREG(SYSCTL + 0x060) = 0x01d40b80;
    /* Wait for PLL to sync, then enable.  */
    while ((HWREG(SYSCTL + 0x050) & 0x40) == 0)
	/* no-op */ ;
    HWREG(SYSCTL + 0x060) &= ~0x0800;

    /* Enable SSI pins (GPIOA 2, 3, 4, 5).  */
    HWREG(GPIOA + 0x420) |= 0x3c; /* HW fn. */
    HWREG(GPIOA + 0x508) |= 0x3c; /* 8mA drive strength.  */
    HWREG(GPIOA + 0x51c) |= 0x3c; /* Digital.  */
    HWREG(GPIOA + 0x510) |= 0x3c; /* Pull-up.  */

    /* D/Cn output for OLED.  */
    HWREG(GPIOC + 0x400) |= 0x80; /* Output */
    HWREG(GPIOC + 0x51c) |= 0x80; /* Digital */
    HWREG(GPIOC + 0x508) |= 0x80; /* 8mA drive strength. */
    HWREG(GPIOC + 0x510) |= 0x80; /* Pull-up.  */
    HWREG(GPIOC + 0x200) = 0x00;

    /* Explicitly enable +15V power to the OLED.
       Needed on rev C board, harmless on earlier boards.  */
    HWREG(GPIOC + 0x400) |= 0x40; /* Output */
    HWREG(GPIOC + 0x51c) |= 0x40; /* Digital */
    HWREG(GPIOC + 0x508) |= 0x40; /* 8mA drive strength. */
    HWREG(GPIOC + 0x510) |= 0x40; /* Pull-up.  */
    HWREG(GPIOC + 0x100) = 0x40; /* Enable.  */

    /* SD card select.  */
    HWREG(GPIOD + 0x400) |= 0x01; /* Output */
    HWREG(GPIOD + 0x51c) |= 0x01; /* Digital */
    HWREG(GPIOD + 0x504) |= 0x01; /* 4mA drive strength. */
    HWREG(GPIOD + 0x510) |= 0x01; /* Pull-up.  */
    HWREG(GPIOD + 0x004) |= 0x01;

    /* User pushbutton input.  */
    HWREG(GPIOF + 0x51c) |= 0x02; /* Digital */
    HWREG(GPIOF + 0x510) |= 0x02; /* Pull-up.  */

    /* Direction buttons.  */
    HWREG(GPIOE + 0x51c) |= 0x0f; /* Digital */
    HWREG(GPIOE + 0x510) |= 0x0f; /* Pull-up.  */

    ssi_flush_rx();

    oled_clear();
    oled_init();
#endif
}

int main(int argc, char **argv)
{
#ifndef LUMINARY
    vwidth = 128;
    vheight = 96;
#endif
    sys_init();
    TimerInit();
    return WolfMain(argc, argv);
}


