#include "healinggunsfx.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "healinggungraphiceffect.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "healingguncameraeffect.h"


HealingGunSFX::HealingGunSFX(ObjectPool *objectPool) :
	PooledObject(objectPool, 0)
{
	this->graphicEffect = new HealingGunGraphicEffect(this);
	//this->soundEffect = new HealingGunSoundEffect(this);
	this->cameraEffect = new HealingGunCameraEffect();
}

void HealingGunSFX::init(IsisActionType mode, HealingGunSFXData *sfxData, Object3D *turret, const Vector3 *localSourcePosition, TankData *tankData)
{
	this->graphicEffect->init(mode, sfxData, turret, localSourcePosition, tankData);
	//this->soundEffect.init(mode, sfxData, turret);
	this->graphicEffectDestroyed = false;
	this->soundEffectDestroyed = false;
}

void HealingGunSFX::setMode(IsisActionType value)
{
	this->graphicEffect->setMode(value);
	//this->soundEffect.mode = value;
}

void HealingGunSFX::addToBattlefield(IBattleField *battlefield)
{
	this->battlefield = battlefield;
	if (this->isLocalTarget() && cameraEffect->refCounter == 1)
	{
		battlefield->addFollowCameraModifier(cameraEffect);
	}
	battlefield->addGraphicEffect(this->graphicEffect);
	//battlefield->addSound3DEffect(this->soundEffect);
}

void HealingGunSFX::destroy()
{
	if (this->isLocalTarget())
		this->cameraEffect->refCounter--;

	this->graphicEffect->kill();
	//this->soundEffect->kill();
	this->targetData = nullptr;
	this->battlefield = nullptr;
	this->storeInPool();
}

void HealingGunSFX::setTargetParams(TankData *targetData,
									TankData *ownerData,
									Object3D *target,
									const Vector3 *localTargetPosition,
									IsisActionType mode)
{
	bool showBeamLight = true;
	if (this->isLocalTarget())
	{
		this->cameraEffect->refCounter--;
	}
	this->targetData = targetData;
	if (this->isLocalTarget())
	{
		this->cameraEffect->refCounter++;
		if (this->battlefield != nullptr && this->cameraEffect->refCounter == 1)
		{
			this->battlefield->addFollowCameraModifier(cameraEffect);
		}
	}
	if (targetData != nullptr)
	{
		showBeamLight = true;
	}
	this->graphicEffect->setTargetParams(target, localTargetPosition, ownerData, mode, showBeamLight);
}

void HealingGunSFX::onEffectDestroyed(ISpecialEffect *effect)
{
	if (effect == this->graphicEffect)
		this->graphicEffectDestroyed = true;

	//if (effect == this->soundEffect)
		//this->soundEffectDestroyed = true;

	if (this->graphicEffectDestroyed && this->soundEffectDestroyed)
	{
		storeInPool();
	}
}


bool HealingGunSFX::isLocalTarget()
{
	return ((!(this->targetData == nullptr)) && (this->targetData == TankData::localTankData));
}
