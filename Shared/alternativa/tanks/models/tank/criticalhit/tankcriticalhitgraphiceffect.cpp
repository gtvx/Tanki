#include "tankcriticalhitgraphiceffect.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "tankcriticalhitmodel.h"
#include "_global.h"


static const int delay = 110;


TankCriticalHitGraphicEffect::TankCriticalHitGraphicEffect(TankData *tankData)
{
	this->tankData = tankData;
}

void TankCriticalHitGraphicEffect::destroy()
{
	this->tankData = nullptr;
}

ClientObjectUser* TankCriticalHitGraphicEffect::getOwner()
{
	return nullptr;
}

bool TankCriticalHitGraphicEffect::play(int millis, GameCamera *camera)
{
	(void)camera;
	if (this->killed)
	{
		return false;
	}
	if (this->time >= delay)
	{
		this->createEffects();
		return false;
	}
	this->time = this->time + millis;
	return true;
}

void TankCriticalHitGraphicEffect::kill()
{
	this->killed = true;
}

void TankCriticalHitGraphicEffect::addToContainer(Scene3DContainer *container)
{
	(void)container;
	this->time = 0;
	this->killed = false;
}


void TankCriticalHitGraphicEffect::createEffects()
{
	_function_name("TankCriticalHitGraphicEffect::createEffects()");

	if (this->tankData == nullptr || this->tankData->tank == nullptr)
		return;

	Tank *tank = this->tankData->tank;
	if (tank->skin->hullMesh->getParent() == nullptr)
	{
		return;
	}

	//TankCriticalHitModel *explosionModel = (Main.osgi.getService(ITankCriticalHitModel) as TankCriticalHitModel);
	TankCriticalHitModel::createExplosionEffects(this->tankData->hull, this->tankData);
}
