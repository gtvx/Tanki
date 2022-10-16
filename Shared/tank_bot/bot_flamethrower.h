#ifndef BOT_FLAMETHROWER_H
#define BOT_FLAMETHROWER_H

#include "bot_shot.h"
#include "alternativa/math/vector3.h"
#include <qglobal.h>

struct FlamethrowerModel;
struct WeaponCommonModel;
class Vector3;
class TankControl;
class Body;


class BotFlamethrower : public BotShot
{
	FlamethrowerModel *flamethrowerModel;
	WeaponCommonModel *weapon;
	Vector3 localBarrelOrigin;
	TankControl *tankControl;
	QVector<Body*> *targetIds;
	double barrelLength;

	bool bot_shoot;
	void startFire();
	void stopFire();
	void tank_current_null();

public:
	BotFlamethrower(TankControl *tankControl);
	~BotFlamethrower();
	bool init();
	void processing(uint32_t time) override;
	void deleteTank(Tank*) override;
};

#endif // BOT_FLAMETHROWER_H
