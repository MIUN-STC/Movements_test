#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "mat.h"


#define BAYER_UNFILTERED 1
#define BAYER_DEMOSAICING 2
#define FILTER_MAP(W,H,X,Y) ((Y) * (W) + (X))

struct RGBA8888
{
	uint8_t A;
	uint8_t B;
	uint8_t G;
	uint8_t R;
};

struct RGB888
{
	uint8_t B;
	uint8_t G;
	uint8_t R;
};

void rgba8888_demosaicing (size_t W, size_t H, uint8_t const * src, struct RGBA8888 * des);
void rgba8888_unfiltered (size_t width, size_t height, uint8_t const * src, struct RGBA8888 * des);
void rgba8888_filter (int filter, size_t width, size_t height, uint8_t const * src, struct RGBA8888 * des);

void rgba8888_demosaicing (size_t W, size_t H, uint8_t const * src, struct RGBA8888 * des)
{
	uint8_t R, G0, G1, B;
	for (size_t X = 0; X < W - 1; X = X + 1)
	{
		for (size_t Y = 0; Y < H - 1; Y = Y + 1)
		{
			if ((X % 2) == 0 && (Y % 2) == 0)
			{
				R =  src [FILTER_MAP (W, H, X + 0, Y + 0)];
				G0 = src [FILTER_MAP (W, H, X + 1, Y + 0)];
				G1 = src [FILTER_MAP (W, H, X + 0, Y + 1)];
				B =  src [FILTER_MAP (W, H, X + 1, Y + 1)];
			}
			if ((X % 2) == 1 && (Y % 2) == 0)
			{
				R =  src [FILTER_MAP (W, H, X + 1, Y + 0)];
				G0 = src [FILTER_MAP (W, H, X + 0, Y + 0)];
				G1 = src [FILTER_MAP (W, H, X + 1, Y + 1)];
				B =  src [FILTER_MAP (W, H, X + 0, Y + 1)];
			}
			if ((X % 2) == 0 && (Y % 2) == 1)
			{
				R =  src [FILTER_MAP (W, H, X + 0, Y + 1)];
				G0 = src [FILTER_MAP (W, H, X + 0, Y + 0)];
				G1 = src [FILTER_MAP (W, H, X + 1, Y + 1)];
				B =  src [FILTER_MAP (W, H, X + 1, Y + 0)];
			}
			if ((X % 2) == 1 && (Y % 2) == 1)
			{
				R =  src [FILTER_MAP (W, H, X + 1, Y + 1)];
				G0 = src [FILTER_MAP (W, H, X + 1, Y + 0)];
				G1 = src [FILTER_MAP (W, H, X + 0, Y + 1)];
				B =  src [FILTER_MAP (W, H, X + 0, Y + 0)];
			}
			des [FILTER_MAP (W, H, X, Y)].R = R;
			des [FILTER_MAP (W, H, X, Y)].G = (G0 + G1) / 2;
			des [FILTER_MAP (W, H, X, Y)].B = B;
			des [FILTER_MAP (W, H, X, Y)].A = 255;
		}
	}
}


void rgba8888_unfiltered (size_t W, size_t H, uint8_t const * src, struct RGBA8888 * des)
{
	for (size_t I = 0; I < (W * H); I = I + 1)
	{
		des [I].R = src [I];
		des [I].G = src [I];
		des [I].B = src [I];
		des [I].A = 255;
	}
}


void rgba8888_filter (int filter, size_t W, size_t H, uint8_t const * src, struct RGBA8888 * des)
{
	switch (filter)
	{
		case BAYER_UNFILTERED:
		rgba8888_unfiltered (W, H, src, des);
		break;
		
		case BAYER_DEMOSAICING:
		rgba8888_demosaicing (W, H, src, des);
		break;
	}
}


/*
void reduce_half 
(
	uint8_t const * src, 
	size_t W1, 
	size_t H1, 
	struct RGBA8888 * des, 
	size_t W2, 
	size_t H2
)
{
	for (size_t X = 0; X < W1 / 2; X = X + 1)
	for (size_t Y = 0; Y < H1 / 2; Y = Y + 1)
	{
		//Assertf (I < (des_width * des_height), "%s", "");
		des [MAP (W2,H2,X,Y)].R = src [MAP (W1,H1,X * 2,Y * 2)];
		des [MAP (W2,H2,X,Y)].G = src [MAP (W1,H1,X * 2,Y * 2)];
		des [MAP (W2,H2,X,Y)].B = src [MAP (W1,H1,X * 2,Y * 2)];
		des [MAP (W2,H2,X,Y)].A = src [MAP (W1,H1,X * 2,Y * 2)];
	}
}
*/
