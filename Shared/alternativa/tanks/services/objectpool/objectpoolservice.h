#ifndef OBJECTPOOLSERVICE_H
#define OBJECTPOOLSERVICE_H

class AnimatedPlaneEffectOld;
class ObjectPool;
class AnimatedSpriteEffectNew;
class StaticObject3DPositionProvider;
class MovingObject3DPositionProvider;
class SpriteShotEffect;
class TankDeathGraphicEffect;
class PlasmaShot;
class DamageUpEffect;
class FlamethrowerGraphicEffect;
class PlaneMuzzleFlashEffect;
class ThunderShotEffect;
class AnimatedSpriteEffect;
class DominationBeamEffect;
class HealingGunSFX;
class MuzzlePositionProvider;
class TubeLightEffect;
class OmniStreamLightEffect;
class TrailEffect1;
class TrailEffect2;
class CollisionObject3DPositionProvider;
class FreezeGraphicEffect;
class BeamEffect;
class ChargeEffect;
class AnimatedLightEffect;
class RicochetShot;
class LightWaveEffect;
class SparkEffect;
class LightBeamEffect;
class LimitedDistanceAnimatedSpriteEffect;
class ScalingObject3DPositionProvider;




class ObjectPoolService
{
	ObjectPool *poolAnimatedPlaneEffectOld;
	ObjectPool *poolAnimatedSpriteEffectNew;
	ObjectPool *poolStaticObject3DPositionProvider;
	ObjectPool *poolMovingObject3DPositionProvider;
	ObjectPool *poolSpriteShotEffect;
	ObjectPool *poolTankDeathGraphicEffect;
	ObjectPool *poolPlasmaShot;
	ObjectPool *poolDamageUpEffect;
	ObjectPool *poolFlamethrowerGraphicEffect;
	ObjectPool *poolPlaneMuzzleFlashEffect;
	ObjectPool *poolThunderShotEffect;
	ObjectPool *poolAnimatedSpriteEffect;
	ObjectPool *poolDominationBeamEffect;
	ObjectPool *poolHealingGunSFX;
	ObjectPool *poolMuzzlePositionProvider;
	ObjectPool *poolTubeLightEffect;
	ObjectPool *poolOmniStreamLightEffect;
	ObjectPool *poolTrailEffect1;
	ObjectPool *poolTrailEffect2;
	ObjectPool *poolCollisionObject3DPositionProvider;
	ObjectPool *poolFreezeGraphicEffect;
	ObjectPool *poolBeamEffect;
	ObjectPool *poolChargeEffect;
	ObjectPool *poolAnimatedLightEffect;
	ObjectPool *poolRicochetShot;
	ObjectPool *poolLightWaveEffect;
	ObjectPool *poolSparkEffect;
	ObjectPool *poolLightBeamEffect;
	ObjectPool *poolLimitedDistanceAnimatedSpriteEffect;
	ObjectPool *poolScalingObject3DPositionProvider;


public:
	ObjectPoolService();
	~ObjectPoolService();

	AnimatedPlaneEffectOld* getAnimatedPlaneEffectOld();
	AnimatedSpriteEffectNew* getAnimatedSpriteEffectNew();
	StaticObject3DPositionProvider* getStaticObject3DPositionProvider();
	MovingObject3DPositionProvider* getMovingObject3DPositionProvider();
	SpriteShotEffect* getSpriteShotEffect();
	TankDeathGraphicEffect* getTankDeathGraphicEffect();
	PlasmaShot* getPlasmaShot();
	DamageUpEffect* getDamageUpEffect();
	FlamethrowerGraphicEffect* getFlamethrowerGraphicEffect();
	PlaneMuzzleFlashEffect* getPlaneMuzzleFlashEffect();
	ThunderShotEffect* getThunderShotEffect();
	AnimatedSpriteEffect* getAnimatedSpriteEffect();
	DominationBeamEffect* getDominationBeamEffect();
	HealingGunSFX* getHealingGunSFX();
	MuzzlePositionProvider* getMuzzlePositionProvider();
	TubeLightEffect* getTubeLightEffect();
	OmniStreamLightEffect* getOmniStreamLightEffect();
	TrailEffect1* getTrailEffect1();
	TrailEffect2* getTrailEffect2();
	CollisionObject3DPositionProvider* getCollisionObject3DPositionProvider();
	FreezeGraphicEffect* getFreezeGraphicEffect();
	BeamEffect* getBeamEffect();
	ChargeEffect* getChargeEffect();
	AnimatedLightEffect* getAnimatedLightEffect();
	RicochetShot* getRicochetShot();
	LightWaveEffect* getLightWaveEffect();
	SparkEffect* getSparkEffect();
	LightBeamEffect* getLightBeamEffect();
	LimitedDistanceAnimatedSpriteEffect* getLimitedDistanceAnimatedSpriteEffect();
	ScalingObject3DPositionProvider* getScalingObject3DPositionProvider();
};

#endif // OBJECTPOOLSERVICE_H
