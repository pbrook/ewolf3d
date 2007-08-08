#include <stdio.h>
#include <stdint.h>

int main()
{
    uint16_t header[3];
    uint32_t offsets[1024];
    uint32_t size;
    int i;
    FILE *f = fopen("vswap.wl6", "rb");
    if (!f)
      return 1;
    fread(header, 2, 3, f);

    printf("#include \"wl_def.h\"\n");
    printf("const myint ChunksInFile = %d;\n", header[0]);
    printf("const myint PMSpriteStart = %d;\n", header[1]);
    printf("#ifdef ENABLE_AUDIO\n");
    printf("const myint PMSoundStart = %d;\n", header[2]);
    printf("pool_id PageAddr[%d];\n", header[0]);
    printf("#else\n");
    printf("pool_id PageAddr[%d];\n", header[2]);
    printf("#endif\n");
    printf("const PageListStruct PMPages[%d] = {\n", header[0]);
    for (i = 0; i < header[0]; i++)
      {
	fread(&offsets[i], 4, 1, f);
	if (offsets[i] & 0xff0000ffu)
	  {
	    fprintf(stderr, "Bad page offset %x\n", offsets[i]);
	    return 1;
	  }
      }
    for (i = 0; i < header[0]; i++)
      {
	fread(&size, 2, 1, f);
	printf("{%d, %d},\n", offsets[i] >> 8, size);
      }
    printf("};\n");
    return 0;
}

