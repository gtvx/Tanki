#ifndef HEALINGGUNGRAPHICEFFECT_H
#define HEALINGGUNGRAPHICEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include <memory>
#include <QVector>

class IHealingGunEffectListener;
enum class IsisActionType;
class HealingGunSFXData;
class Object3D;
class Vector3;
class TankData;
class Shaft;
class AnimatedSprite3D;
class OmniStreamLightEffect;
class IBattleField;
class TubeLightEffect;
class TextureAnimation;
class Material;
class HealingGunShaft;


class HealingGunGraphicEffect : public IGraphicEffect
{
	IBattleField *bfModel;
	Scene3DContainer *container;
	std::shared_ptr<HealingGunShaft> shaftPlane;
	std::shared_ptr<AnimatedSprite3D> spark;
	std::shared_ptr<AnimatedSprite3D> shaftEnd;
	Vector3 *startPosition;
	Object3D *turret;
	Vector3 *localSourcePosition;
	Object3D *targetObject;
	Vector3 *localTargetPosition;
	bool dead;
	int currentFrame;
	double frameRate = 0.015;
	int time;
	IsisActionType _mode;
	HealingGunSFXData *sfxData;
	TextureAnimation *sparkMaterials;

	//shaftMaterials:Vector.<Material>;
	QVector<std::shared_ptr<Material>>* shaftMaterials;
	QVector<std::shared_ptr<Material>>* shaftEndMaterials;
	int shaftEndFrame;
	IHealingGunEffectListener *listener;
	OmniStreamLightEffect *lightEffect;
	TubeLightEffect *beamEffect;
	IsisActionType stateLightEffectCreatedFor;
	TankData *targetTankData;
public:
	HealingGunGraphicEffect(IHealingGunEffectListener *listener);

	void createGraphics();
	void init(IsisActionType mode, HealingGunSFXData *sfxData, Object3D *turret, const Vector3 *localSourcePosition, TankData *targetData);

private:
	void createLightEffect(IsisActionType param1, TankData *targetData);

public:
	void setMode(IsisActionType value);
	void setTargetParams(Object3D *targetObject, const Vector3 *localTargetPosition, TankData *targetTankData, IsisActionType mode, bool showBeamLight);
	ClientObjectUser* getOwner();
	bool play(int millis, GameCamera *camera);
	void addToContainer(Scene3DContainer *container);
	void destroy();
	void kill();
	void updateShaft(GameCamera *camera);
	void createBeamLight(IsisActionType param1, TankData *targetTankData, const Vector3 *localTargetPosition);
	void stopLightEffect();
	void updateMode();
	void setIdleMode();
	void setHealMode();
	void setDamageMode();
};

#endif // HEALINGGUNGRAPHICEFFECT_H
