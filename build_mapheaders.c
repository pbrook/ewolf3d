#include <stdio.h>
#include <stdint.h>

int main()
{
    uint32_t offsets[60];
    struct {
	uint32_t offset[3];
	uint16_t size[3];
    } __attribute__((packed)) data;
    int i;
    uint16_t rlew;
    unsigned char *p;
    FILE *f = fopen("maphead.wl6", "rb");
    fread(&rlew, 2, 1, f);
    if (!f)
      return 1;
    fread(offsets, 4, 60, f);
    fclose(f);

    printf("#include \"wl_def.h\"\n");
    printf("const word RLEWtag = 0x%x;\n", rlew);
    printf("const maptype mapheaderseg[60] = {\n");
    f = fopen("gamemaps.wl6", "rb");
    for (i = 0; i < 60; i++)
      {
	fseek(f, offsets[i], SEEK_SET);
	fread(&data, 2, 9, f);
	printf("{{%d, %d}, {%d, %d}},\n",
	       data.offset[0], data.offset[1], data.size[0], data.size[1]);
      }
    printf("};\n");
    return 0;
}

