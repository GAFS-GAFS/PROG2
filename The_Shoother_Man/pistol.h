#ifndef __PISTOL__ 																												
#define __PISTOL__																												

#include "bullet.h"

#define PISTOL_COOLDOWN 15																									

typedef struct {																												
	unsigned char timer;																										
	bullet *shots;																													
} pistol;																														


pistol* createPistol();

bullet* firePistol(unsigned short x, unsigned short y, unsigned char trajectory, pistol *gun);									

void destroyPistol(pistol *element);																							

#endif	