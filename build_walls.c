#include "wl_def.h"

byte chunk[4096];
FILE *vswap;
int orgsize;

int total_rle;
int total_lz;
int total_before;

void compress_wall(int picnum)
{
    byte pixels[64 * 64];
    byte data[16];
    int i;
    int j;
    int n;
    byte *p;
    int bestmatch;
    int bestpos;
    int val[4];
    byte c;

    if (PMPages[picnum].length == 0) {
	printf("#define Wall%d NULL\n", picnum);
	return;
    }
    fseek(vswap, PMPages[picnum].offset << 8, SEEK_SET);
    fread(pixels, PMPages[picnum].length, 1, vswap);
    for (i = 0; i < 64 * 64; i++) {
	pixels[i] = pal4bit[pixels[i]];
    }
    printf ("static const byte ROMAREA Wall%d[64 * 18] = {\n", picnum);
    p = pixels;
    for (i = 0; i < 64; i++) {
	for (j = 0; j < 16; j++)
	    data[j] = 0;
	for (j = 0; j < 64; j++)
	    data[p[j]]++;
	for (n = 0; n < 4; n++) {
	    bestmatch = 0;
	    bestpos = 0;
	    for (j = 0; j < 16; j++) {
		if (bestmatch < data[j]) {
		    bestmatch = data[j];
		    bestpos = j;
		}
	    }
	    val[n] = bestpos;
	    data[bestpos] = 0;
	    if (bestpos < 15)
		data[bestpos + 1] = (data[bestpos + 1] + 1) >> 1;
	    if (bestpos > 0)
		data[bestpos - 1] = (data[bestpos - 1] + 1) >> 1;
	}
	printf ("%d, %d, ", (val[0] << 4) | val[1], (val[2] << 4) |  val[3]);
	c = 0;
	for (j = 0; j < 64; j++) {
	    bestmatch = 16;
	    bestpos = 0;
	    for (n = 0; n < 4; n++) {
		if (abs(p[j] - val[n]) < bestmatch) {
		    bestmatch = abs(p[j] - val[n]);
		    bestpos = n;
		}
	    }
	    c |= (bestpos << ((j & 3) * 2));
	    if ((j & 3) == 3) {
		printf("%d, ", c);
		c = 0;
	    }
	}
	printf ("\n");
	p += 64;
    }
    printf("};\n");
}

int main()
{
  int i;
  vswap = fopen("vswap." GAMEEXT, "rb");
  printf("#include \"wl_def.h\"\n");
  for (i = 0; i < PMSpriteStart; i++) {
      compress_wall(i);
  }
  // FIXME: Could use array indices instead of pointers.  */
  printf("const byte *const ROMAREA WallChunks[%d] = {\n", PMSpriteStart);
  for (i = 0; i < PMSpriteStart; i++) {
      printf("  Wall%d,\n", i);
  }
  printf("};\n");
  fclose(vswap);
  return 0;
}

