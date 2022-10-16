#include "aimedshotresult.h"
#include "aim.h"

AimedShotResult::AimedShotResult()
{
	this->isStatic = false;
}

void AimedShotResult::setTarget(Tank *tank, const Vector3 *targetHitPoint)
{
	this->targetHitPoint.copy(targetHitPoint);
	this->aims.append(new Aim(tank, targetHitPoint));
}

void AimedShotResult::setStaticHitPoint(const Vector3 *staticHitPoint)
{
	this->isStatic = true;
	this->staticHitPoint.copy(staticHitPoint);
}
