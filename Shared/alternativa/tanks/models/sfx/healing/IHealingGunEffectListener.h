#ifndef IHEALINGGUNEFFECTLISTENER_H
#define IHEALINGGUNEFFECTLISTENER_H

class ISpecialEffect;

class IHealingGunEffectListener
{
public:
	 virtual void onEffectDestroyed(ISpecialEffect*) = 0;
};

#endif // IHEALINGGUNEFFECTLISTENER_H
