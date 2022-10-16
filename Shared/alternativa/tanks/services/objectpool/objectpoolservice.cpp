#include "objectpoolservice.h"
#include "__global.h"
#include "alternativa/tanks/sfx/animatedplaneeffectold.h"
#include "alternativa/tanks/utils/objectpool/objectpool.h"
#include "alternativa/tanks/sfx/animatedspriteeffectnew.h"
#include "alternativa/tanks/sfx/staticobject3dpositionprovider.h"
#include "alternativa/tanks/sfx/movingobject3dpositionprovider.h"
#include "alternativa/tanks/models/tank/explosion/tankdeathgraphiceffect.h"
#include "alternativa/tanks/models/sfx/spriteshoteffect.h"
#include "alternativa/tanks/models/weapon/plasma/plasmashot.h"
#include "alternativa/tanks/models/battlefield/effects/damageupeffect.h"
#include "alternativa/tanks/models/sfx/flame/flamethrowergraphiceffect.h"
#include "alternativa/tanks/sfx/planemuzzleflasheffect.h"
#include "alternativa/tanks/models/sfx/shoot/thunder/thundershoteffect.h"
#include "alternativa/tanks/sfx/animatedspriteeffect.h"
#include "alternativa/tanks/models/dom/sfx/dominationbeameffect.h"
#include "alternativa/tanks/models/sfx/healing/healinggunsfx.h"
#include "alternativa/tanks/models/sfx/muzzlepositionprovider.h"
#include "alternativa/tanks/models/sfx/tubelighteffect.h"
#include "alternativa/tanks/models/sfx/omnistreamlighteffect.h"
#include "alternativa/tanks/models/sfx/shoot/shaft/traileffect1.h"
#include "alternativa/tanks/models/sfx/shoot/shaft/traileffect2.h"
#include "alternativa/tanks/models/sfx/collisionobject3dpositionprovider.h"
#include "alternativa/tanks/models/weapon/freeze/sfx/freezegraphiceffect.h"
#include "alternativa/tanks/models/sfx/shoot/railgun/beameffect.h"
#include "alternativa/tanks/models/sfx/shoot/railgun/chargeeffect.h"
#include "alternativa/tanks/models/sfx/animatedlighteffect.h"
#include "alternativa/tanks/models/weapon/ricochet/ricochetshot.h"
#include "alternativa/tanks/models/battlefield/effects/levelup/lightwaveeffect.h"
#include "alternativa/tanks/models/battlefield/effects/levelup/sparkeffect.h"
#include "alternativa/tanks/models/battlefield/effects/levelup/lightbeameffect.h"
#include "alternativa/tanks/sfx/limiteddistanceanimatedspriteeffect.h"
#include "alternativa/tanks/sfx/scalingobject3dpositionprovider.h"




class PooledObject;

ObjectPoolService::ObjectPoolService()
{
	poolAnimatedPlaneEffectOld = new ObjectPool;
	poolAnimatedSpriteEffectNew = new ObjectPool;
	poolStaticObject3DPositionProvider = new ObjectPool;
	poolMovingObject3DPositionProvider = new ObjectPool;
	poolSpriteShotEffect = new ObjectPool;
	poolTankDeathGraphicEffect = new ObjectPool;
	poolPlasmaShot = new ObjectPool;
	poolDamageUpEffect = new ObjectPool;
	poolFlamethrowerGraphicEffect = new ObjectPool;
	poolPlaneMuzzleFlashEffect = new ObjectPool;
	poolThunderShotEffect = new ObjectPool;
	poolAnimatedSpriteEffect = new ObjectPool;
	poolDominationBeamEffect = new ObjectPool;
	poolHealingGunSFX = new ObjectPool;
	poolMuzzlePositionProvider = new ObjectPool;
	poolTubeLightEffect = new ObjectPool;
	poolOmniStreamLightEffect = new ObjectPool;
	poolTrailEffect1 = new ObjectPool;
	poolTrailEffect2 = new ObjectPool;
	poolCollisionObject3DPositionProvider = new ObjectPool;
	poolFreezeGraphicEffect = new ObjectPool;
	poolBeamEffect = new ObjectPool;
	poolChargeEffect = new ObjectPool;
	poolAnimatedLightEffect = new ObjectPool;
	poolRicochetShot = new ObjectPool;
	poolLightWaveEffect = new ObjectPool;
	poolSparkEffect = new ObjectPool;
	poolLightBeamEffect = new ObjectPool;
	poolLimitedDistanceAnimatedSpriteEffect = new ObjectPool;
	poolScalingObject3DPositionProvider = new ObjectPool;
}

ObjectPoolService::~ObjectPoolService()
{
	delete poolAnimatedPlaneEffectOld;
	delete poolAnimatedSpriteEffectNew;
	delete poolStaticObject3DPositionProvider;
	delete poolMovingObject3DPositionProvider;
	delete poolSpriteShotEffect;
	delete poolTankDeathGraphicEffect;
	delete poolPlasmaShot;
	delete poolDamageUpEffect;
	delete poolFlamethrowerGraphicEffect;
	delete poolPlaneMuzzleFlashEffect;
	delete poolThunderShotEffect;
	delete poolAnimatedSpriteEffect;
	delete poolDominationBeamEffect;
	delete poolHealingGunSFX;
	delete poolMuzzlePositionProvider;
	delete poolTubeLightEffect;
	delete poolOmniStreamLightEffect;
	delete poolTrailEffect1;
	delete poolTrailEffect2;
	delete poolCollisionObject3DPositionProvider;
	delete poolFreezeGraphicEffect;
	delete poolBeamEffect;
	delete poolChargeEffect;
	delete poolAnimatedLightEffect;
	delete poolRicochetShot;
	delete poolLightWaveEffect;
	delete poolSparkEffect;
	delete poolLightBeamEffect;
	delete poolLimitedDistanceAnimatedSpriteEffect;
	delete poolScalingObject3DPositionProvider;
}


AnimatedPlaneEffectOld* ObjectPoolService::getAnimatedPlaneEffectOld()
{
#ifdef GRAPHICS
	if (poolAnimatedPlaneEffectOld->list.isEmpty())
		return new AnimatedPlaneEffectOld(poolAnimatedPlaneEffectOld);
	else
		return (AnimatedPlaneEffectOld*)poolAnimatedPlaneEffectOld->list.takeLast();
#endif
	return nullptr;
}

AnimatedSpriteEffectNew* ObjectPoolService::getAnimatedSpriteEffectNew()
{
#ifdef GRAPHICS
	if (poolAnimatedSpriteEffectNew->list.isEmpty())
		return new AnimatedSpriteEffectNew(poolAnimatedSpriteEffectNew);
	else
		return (AnimatedSpriteEffectNew*)poolAnimatedSpriteEffectNew->list.takeLast();
#endif
	return nullptr;
}

StaticObject3DPositionProvider* ObjectPoolService::getStaticObject3DPositionProvider()
{
#ifdef GRAPHICS
	if (poolStaticObject3DPositionProvider->list.isEmpty())
		return new StaticObject3DPositionProvider(poolStaticObject3DPositionProvider);
	else
		return (StaticObject3DPositionProvider*)poolStaticObject3DPositionProvider->list.takeLast();
#endif
	return nullptr;
}

MovingObject3DPositionProvider* ObjectPoolService::getMovingObject3DPositionProvider()
{
#ifdef GRAPHICS
	if (poolMovingObject3DPositionProvider->list.isEmpty())
		return new MovingObject3DPositionProvider(poolMovingObject3DPositionProvider);
	else
		return (MovingObject3DPositionProvider*)poolMovingObject3DPositionProvider->list.takeLast();
#endif
	return nullptr;
}

SpriteShotEffect* ObjectPoolService::getSpriteShotEffect()
{
	if (poolSpriteShotEffect->list.isEmpty())
		return new SpriteShotEffect(poolSpriteShotEffect);
	else
		return (SpriteShotEffect*)poolSpriteShotEffect->list.takeLast();
}

TankDeathGraphicEffect* ObjectPoolService::getTankDeathGraphicEffect()
{
	if (poolTankDeathGraphicEffect->list.isEmpty())
		return new TankDeathGraphicEffect(poolTankDeathGraphicEffect);
	else
		return (TankDeathGraphicEffect*)poolTankDeathGraphicEffect->list.takeLast();
}

PlasmaShot* ObjectPoolService::getPlasmaShot()
{
	if (poolPlasmaShot->list.isEmpty())
		return new PlasmaShot(poolPlasmaShot);
	else
		return (PlasmaShot*)poolPlasmaShot->list.takeLast();
}

DamageUpEffect* ObjectPoolService::getDamageUpEffect()
{
#ifdef GRAPHICS
	if (poolDamageUpEffect->list.isEmpty())
		return new DamageUpEffect(poolDamageUpEffect);
	else
		return (DamageUpEffect*)poolDamageUpEffect->list.takeLast();
#endif
	return nullptr;
}

FlamethrowerGraphicEffect* ObjectPoolService::getFlamethrowerGraphicEffect()
{
#ifdef GRAPHICS
	if (poolFlamethrowerGraphicEffect->list.isEmpty())
		return new FlamethrowerGraphicEffect(poolFlamethrowerGraphicEffect);
	else
		return (FlamethrowerGraphicEffect*)poolFlamethrowerGraphicEffect->list.takeLast();
#endif
	return nullptr;
}

PlaneMuzzleFlashEffect* ObjectPoolService::getPlaneMuzzleFlashEffect()
{
#ifdef GRAPHICS
	if (poolPlaneMuzzleFlashEffect->list.isEmpty())
		return new PlaneMuzzleFlashEffect(poolPlaneMuzzleFlashEffect);
	else
		return (PlaneMuzzleFlashEffect*)poolPlaneMuzzleFlashEffect->list.takeLast();
#endif
	return nullptr;
}

ThunderShotEffect* ObjectPoolService::getThunderShotEffect()
{
#ifdef GRAPHICS
	if (poolThunderShotEffect->list.isEmpty())
		return new ThunderShotEffect(poolThunderShotEffect);
	else
		return (ThunderShotEffect*)poolThunderShotEffect->list.takeLast();
#endif
	return nullptr;
}

AnimatedSpriteEffect* ObjectPoolService::getAnimatedSpriteEffect()
{
#ifdef GRAPHICS
	if (poolAnimatedSpriteEffect->list.isEmpty())
		return new AnimatedSpriteEffect(poolAnimatedSpriteEffect);
	else
		return (AnimatedSpriteEffect*)poolAnimatedSpriteEffect->list.takeLast();
#endif
	return nullptr;
}

DominationBeamEffect* ObjectPoolService::getDominationBeamEffect()
{
#ifdef GRAPHICS
	if (poolDominationBeamEffect->list.isEmpty())
		return new DominationBeamEffect(poolDominationBeamEffect);
	else
		return (DominationBeamEffect*)poolDominationBeamEffect->list.takeLast();
#endif
	return nullptr;
}

HealingGunSFX* ObjectPoolService::getHealingGunSFX()
{
#ifdef GRAPHICS
	if (poolHealingGunSFX->list.isEmpty())
		return new HealingGunSFX(poolHealingGunSFX);
	else
		return (HealingGunSFX*)poolHealingGunSFX->list.takeLast();
#endif
	return nullptr;
}

MuzzlePositionProvider* ObjectPoolService::getMuzzlePositionProvider()
{
#ifdef GRAPHICS
	if (poolMuzzlePositionProvider->list.isEmpty())
		return new MuzzlePositionProvider(poolMuzzlePositionProvider);
	else
		return (MuzzlePositionProvider*)poolMuzzlePositionProvider->list.takeLast();
#endif
	return nullptr;
}

TubeLightEffect* ObjectPoolService::getTubeLightEffect()
{
#ifdef GRAPHICS
	if (poolTubeLightEffect->list.isEmpty())
		return new TubeLightEffect(poolTubeLightEffect);
	else
		return (TubeLightEffect*)poolTubeLightEffect->list.takeLast();
#endif
	return nullptr;
}

OmniStreamLightEffect* ObjectPoolService::getOmniStreamLightEffect()
{
#ifdef GRAPHICS
	if (poolOmniStreamLightEffect->list.isEmpty())
		return new OmniStreamLightEffect(poolOmniStreamLightEffect);
	else
		return (OmniStreamLightEffect*)poolOmniStreamLightEffect->list.takeLast();
#endif
	return nullptr;
}

TrailEffect1* ObjectPoolService::getTrailEffect1()
{
#ifdef GRAPHICS
	if (poolTrailEffect1->list.isEmpty())
		return new TrailEffect1(poolTrailEffect1);
	else
		return (TrailEffect1*)poolTrailEffect1->list.takeLast();
#endif
	return nullptr;
}

TrailEffect2* ObjectPoolService::getTrailEffect2()
{
#ifdef GRAPHICS
	if (poolTrailEffect2->list.isEmpty())
		return new TrailEffect2(poolTrailEffect2);
	else
		return (TrailEffect2*)poolTrailEffect2->list.takeLast();
#endif
	return nullptr;
}

CollisionObject3DPositionProvider* ObjectPoolService::getCollisionObject3DPositionProvider()
{
#ifdef GRAPHICS
	if (poolCollisionObject3DPositionProvider->list.isEmpty())
		return new CollisionObject3DPositionProvider(poolCollisionObject3DPositionProvider);
	else
		return (CollisionObject3DPositionProvider*)poolCollisionObject3DPositionProvider->list.takeLast();
#endif
	return nullptr;
}

FreezeGraphicEffect* ObjectPoolService::getFreezeGraphicEffect()
{
#ifdef GRAPHICS
	if (poolFreezeGraphicEffect->list.isEmpty())
		return new FreezeGraphicEffect(poolFreezeGraphicEffect);
	else
		return (FreezeGraphicEffect*)poolFreezeGraphicEffect->list.takeLast();
#endif
	return nullptr;
}

BeamEffect* ObjectPoolService::getBeamEffect()
{
#ifdef GRAPHICS
	if (poolBeamEffect->list.isEmpty())
		return new BeamEffect(poolBeamEffect);
	else
		return (BeamEffect*)poolBeamEffect->list.takeLast();
#endif
	return nullptr;
}

ChargeEffect* ObjectPoolService::getChargeEffect()
{
#ifdef GRAPHICS
	if (poolChargeEffect->list.isEmpty())
		return new ChargeEffect(poolChargeEffect);
	else
		return (ChargeEffect*)poolChargeEffect->list.takeLast();
#endif
	return nullptr;
}

AnimatedLightEffect* ObjectPoolService::getAnimatedLightEffect()
{
#ifdef GRAPHICS
	if (poolAnimatedLightEffect->list.isEmpty())
		return new AnimatedLightEffect(poolAnimatedLightEffect);
	else
		return (AnimatedLightEffect*)poolAnimatedLightEffect->list.takeLast();
#endif
	return nullptr;
}

RicochetShot* ObjectPoolService::getRicochetShot()
{
	if (poolRicochetShot->list.isEmpty())
		return new RicochetShot(poolRicochetShot);
	else
		return (RicochetShot*)poolRicochetShot->list.takeLast();
}

LightWaveEffect* ObjectPoolService::getLightWaveEffect()
{
#ifdef GRAPHICS
	if (poolLightWaveEffect->list.isEmpty())
		return new LightWaveEffect(poolLightWaveEffect);
	else
		return (LightWaveEffect*)poolLightWaveEffect->list.takeLast();
#endif
	return nullptr;
}

SparkEffect* ObjectPoolService::getSparkEffect()
{
#ifdef GRAPHICS
	if (poolSparkEffect->list.isEmpty())
		return new SparkEffect(poolSparkEffect);
	else
		return (SparkEffect*)poolSparkEffect->list.takeLast();
#endif
	return nullptr;
}

LightBeamEffect* ObjectPoolService::getLightBeamEffect()
{
#ifdef GRAPHICS
	if (poolLightBeamEffect->list.isEmpty())
		return new LightBeamEffect(poolLightBeamEffect);
	else
		return (LightBeamEffect*)poolLightBeamEffect->list.takeLast();
#endif
	return nullptr;
}

LimitedDistanceAnimatedSpriteEffect* ObjectPoolService::getLimitedDistanceAnimatedSpriteEffect()
{
#ifdef GRAPHICS
	if (poolLimitedDistanceAnimatedSpriteEffect->list.isEmpty())
		return new LimitedDistanceAnimatedSpriteEffect(poolLimitedDistanceAnimatedSpriteEffect);
	else
		return (LimitedDistanceAnimatedSpriteEffect*)poolLimitedDistanceAnimatedSpriteEffect->list.takeLast();
#endif
	return nullptr;
}

ScalingObject3DPositionProvider* ObjectPoolService::getScalingObject3DPositionProvider()
{
#ifdef GRAPHICS
	if (poolScalingObject3DPositionProvider->list.isEmpty())
		return new ScalingObject3DPositionProvider(poolScalingObject3DPositionProvider);
	else
		return (ScalingObject3DPositionProvider*)poolScalingObject3DPositionProvider->list.takeLast();
#endif
	return nullptr;
}

