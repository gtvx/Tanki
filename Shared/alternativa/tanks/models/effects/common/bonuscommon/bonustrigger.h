#ifndef BONUSTRIGGER_H
#define BONUSTRIGGER_H

#include "alternativa/tanks/battle/Trigger.h"

class ParaBonus2;
class CollisionBox;
class BattlefieldModel;
class Vector3;
class Matrix3;


class BonusTrigger : public Trigger
{
public:
	ParaBonus2 *bonus;
	CollisionBox *collisionBox;
	BattlefieldModel *battleRunner;

	BonusTrigger(ParaBonus2 *bonus);
	~BonusTrigger();

	void activate(BattlefieldModel *battleRunner);
	void deactivate();
	void update(double x, double y, double z, double rx, double ry, double rz);
	void setTransform(const Vector3 *position, const Matrix3 *m);
	void checkTrigger(Body *body);
};

#endif // BONUSTRIGGER_H
