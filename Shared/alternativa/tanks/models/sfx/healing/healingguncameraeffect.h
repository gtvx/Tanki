#ifndef HEALINGGUNCAMERAEFFECT_H
#define HEALINGGUNCAMERAEFFECT_H

#include "alternativa/tanks/camera/ICameraStateModifier.h"

class HealingGunCameraEffect : public ICameraStateModifier
{
	int startTime;
public:
	int refCounter;

	HealingGunCameraEffect();

	bool update(int time, int delta, Vector3 *position, Vector3 *rotation);
	void onAddedToController(IFollowCameraController *controller);
	void destroy();
};

#endif // HEALINGGUNCAMERAEFFECT_H
