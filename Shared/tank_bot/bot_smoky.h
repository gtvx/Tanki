#ifndef BOT_SMOKY_H
#define BOT_SMOKY_H


#include "bot_shot.h"
#include "alternativa/math/vector3.h"

struct SmokyModel;
struct Vector3;
class TankControl;

class BotSmoky : public BotShot
{
	SmokyModel *smokyModel;
	Vector3 localBarrelOrigin;
	TankControl *tankControl;

public:

	BotSmoky(TankControl *tankControl);
	~BotSmoky();

	bool init();
	void processing(uint32_t time) override;
	void deleteTank(Tank *tank) override;
};


#endif // BOT_SMOKY_H
