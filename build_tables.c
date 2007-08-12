#define finetangent const_finetangent
#define sintable const_sintable
#include "wl_def.h"
#undef finetangent
#undef sintable

fixed finetangent[FINEANGLES/4];
fixed sintable[ANGLES+ANGLES/4+1];

static const double radtoint = (double)FINEANGLES/2.0/PI;

int main()
{
	myint i;
	double tang, angle, anglestep;
	fixed value;

	/* FIXME: Use linear interpolation to reduce size of trig tables. */
/* calculate fine tangents */

	finetangent[0] = 0;
	for (i = 1; i < FINEANGLES/8; i++) {
		tang = tan((double)i/radtoint);
		finetangent[i] = tang*TILEGLOBAL;
		finetangent[FINEANGLES/4-1-i] = TILEGLOBAL/tang;
	}
	
	/* fight off asymptotic behaviour at 90 degrees */
	finetangent[FINEANGLES/4-1] = finetangent[FINEANGLES/4-2]+1;
	
	printf ("#include \"wl_def.h\"\n");
	printf ("const fixed finetangent[] = {\n");
	printf ("  0");
	for (i = 1; i < FINEANGLES/4; i++) {
	    printf (",\n  0x%x", finetangent[i]);
	}
	printf ("\n};\n");
//
// costable overlays sintable with a quarter phase shift
// ANGLES is assumed to be divisable by four
//

	angle = 0.0;
	anglestep = PI/2.0/ANGLEQUAD;
	printf ("const uint16_t sintable[] = {\n");
	printf ("  0");
	for (i = 0; i < ANGLEQUAD; i++) {
	    value = GLOBAL1*sin(angle);
	    printf (",\n  0x%x", value);
	    angle += anglestep;
	}
	printf ("\n};\n");
	return 0;
}

