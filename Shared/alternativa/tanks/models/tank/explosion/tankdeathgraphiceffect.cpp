#include "tankdeathgraphiceffect.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/physics/bodystate.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "_global.h"
#include "tankexplosionmodel.h"


//private static var delay:ConsoleVarInt = new ConsoleVarInt("tankexpl_goffset", 110, 0, 2000);

static const int delay = 110;


TankDeathGraphicEffect::TankDeathGraphicEffect(ObjectPool *objectPool)
	: PooledObject(objectPool, 3)
{

}

TankDeathGraphicEffect::~TankDeathGraphicEffect()
{

}

void TankDeathGraphicEffect::init(TankData *tankData)
{
	this->tankData = tankData;
	this->time = 0;
	this->killed = false;
}

void TankDeathGraphicEffect::destroy()
{
	this->tankData = nullptr;
	this->storeInPool();
}

ClientObjectUser *TankDeathGraphicEffect::getOwner()
{
	return tankData->user;
}

bool TankDeathGraphicEffect::play(int millis, GameCamera *camera)
{
	(void)camera;
	if (this->killed)
		return false;

	if (this->time >= delay)
	{
		this->createEffects();
		return false;
	}

	this->time += millis;
	return true;
}

void TankDeathGraphicEffect::kill()
{
	this->killed = true;
}

void TankDeathGraphicEffect::addToContainer(Scene3DContainer *container)
{
	(void)container;
	this->time = 0;
	this->killed = false;
}

void TankDeathGraphicEffect::createEffects()
{
	_function_name("TankDeathGraphicEffect::createEffects");
	Tank *tank = this->tankData->tank;
	//if (tank->skin->getHullMesh()->parent == nullptr)
		//return;
	tank->state.velocity.z += 500.;
	tank->state.rotation.reset(2., 2., 2.);
	if (tank->skin != nullptr)
		tank->skin->setDeadState();
	tank->skin->resetColorTransform();
#ifdef GRAPHICS
	TankExplosionModel::createExplosionEffects(this->tankData->hull, this->tankData);
#endif
}
