#ifndef __PISTOL__
#define __PISTOL__

#include "bullet.h"

#define PISTOL_COOLDOWN 15

typedef struct
{
	unsigned char timer;
	bullet *shots;
	// Sistema de munição/estamina
	int ammo;		  // balas atuais no pente
	int max_ammo;	  // balas máximas no pente
	int reloading;	  // 1 = recarregando, 0 = não
	int reload_timer; // frames restantes para recarga
} pistol;

pistol *createPistol();

bullet *firePistol(unsigned short x, unsigned short y, unsigned char trajectory, pistol *gun, int is_player);

void destroyPistol(pistol *element);

#endif