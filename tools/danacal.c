#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <gem.h>
#include <mint/osbind.h>

static short input[128];
static short output[128];
static short wk;

static short xmax;
static short ymax;
static long sx[3], sy[3];
static long tx[3], ty[3];
static int target;

static void line(short x1, short y1, short x2, short y2)
{
	input[0] = x1;
	input[1] = y1;
	input[2] = x2;
	input[3] = y2;
	v_pline(wk, 2, input);
}

static void crosshair(short x, short y, bool highlighted)
{
	line(x-10, y, x+10, y);
	line(x, y-10, x, y+10);
	
	if (highlighted)
	{
		input[0] = x-15;
		input[1] = y-15;
		input[2] = x+15;
		input[3] = y+15;
		v_rbox(wk, input);
	}
}

static void center(short x, short y, const char* msg)
{
	vqt_extent(wk, msg, output);
	short w = output[2] - output[0];
	short h = output[3] - output[1];

	v_gtext(wk, x-w/2, y-h/2, msg);
}

static void redraw(void)
{
	v_clrwk(wk);
	for (int i=0; i<3; i++)
		crosshair(sx[i], sy[i], target==i);

	center(xmax/2, ymax/3, "Touch highlighted targets to calibrate touchscreen");
}

int main(int argc, const char* argv[])
{
	short appHandle = appl_init();
	short grafHandle = graf_handle(NULL, NULL, NULL, NULL);

	input[0] = 1; /* default screen type */
	input[1] = input[2] = input[3] = input[4] = input[5]
		= input[6] = input[7] = input[8] = input[9] = 1;
	input[10] = 2; /* RC coordinates */
	v_opnvwk(input, &wk, output);
	xmax = output[0];
	ymax = output[1];

	sx[0] = xmax/10;
	sy[0] = ymax/10;

	sx[1] = xmax/5;
	sy[1] = ymax - ymax/10;

	sx[2] = xmax - xmax/10;
	sy[2] = ymax/2;

	target = 0;
	v_hide_c(wk);
	redraw();

	for (;;)
	{
		short pstatus, x, y;
		vq_mouse(wk, &pstatus, &x, &y);
		if (pstatus)
		{
			/* Read touchscreen */

			(void)trap_14_wlll((short)0x8e, &x, &y, &pstatus);
			tx[target] = x;
			ty[target] = y;

			/* Wait for the user to let go again */

			for (;;)
			{
				vq_mouse(wk, &pstatus, &x, &y);
				if (!pstatus)
					break;
			}

			target++;
			if (target == 3)
				break;

			redraw();
		}
	}

	long c[7];
	c[0] = (tx[0]-tx[2])*(ty[1]-ty[2]) - (tx[1]-tx[2])*(ty[0]-ty[2]);
	c[1] = (sx[0]-sx[2])*(ty[1]-ty[2]) - (sx[1]-sx[2])*(ty[0]-ty[2]);
	c[2] = (tx[0]-tx[2])*(sx[1]-sx[2]) - (sx[0]-sx[2])*(tx[1]-tx[2]);
	c[3] = ty[0]*(tx[2]*sx[1] - tx[1]*sx[2])
			+ ty[1]*(tx[0]*sx[2] - tx[2]*sx[0])
			+ ty[2]*(tx[1]*sx[0] - tx[0]*sx[1]);
	c[4] = (sy[0]-sy[2])*(ty[1]-ty[2]) - (sy[1]-sy[2])*(ty[0]-ty[2]);
	c[5] = (tx[0]-tx[2])*(sy[1]-sy[2]) - (sy[0]-sy[2])*(tx[1]-tx[2]);
	c[6] = ty[0]*(tx[2]*sy[1] - tx[1]*sy[2])
			+ ty[1]*(tx[0]*sy[2] - tx[2]*sy[0])
			+ ty[2]*(tx[1]*sy[0] - tx[0]*sy[1]);

	/* Set coefficients */

	(void)trap_14_wl((short)0x8f, c);
	return 0;
}

