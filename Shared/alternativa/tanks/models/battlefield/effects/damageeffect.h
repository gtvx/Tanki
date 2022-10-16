#ifndef DAMAGEEFFECT_H
#define DAMAGEEFFECT_H

#include <stdint.h>

class Tank;
class DamageTextures;

class DamageEffect
{
	DamageTextures *damageTextures;

public:

	DamageEffect();
	~DamageEffect();

	void createEffect(Tank *tank, int damage, uint32_t color);
};

#endif // DAMAGEEFFECT_H
