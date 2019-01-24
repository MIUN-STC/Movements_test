#pragma once

#include <math.h>
#include "cam.h"
#include "common.h"

void ctrl_auto_exposure2 (float * exposure, struct Gev_Image * image, uint8_t target)
{
	float x = (*exposure);
	float d = 0;
	float n = MAX (1, (float)image->recv_size) * 256;
	for (size_t I = 0; I < image->recv_size; I = I + 1)
	{
		d += (float)(target - image->address [I]);
	}
	d = d / n;
	if (fabs (d) < 0.05f) {d = 0.0f;}
	x += (d * x);
	x = MAX (26, x);
	x = MIN (1000000, x);
	(*exposure) = x;
}
