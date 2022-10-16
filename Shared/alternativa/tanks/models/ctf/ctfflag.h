#ifndef CTFFLAG_H
#define CTFFLAG_H

#include "alternativa/math/vector3.h"
#include "ctfflagstate.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include <memory>

class TankData;
class CollisionPrimitive;
class BitmapData;
class TanksCollisionDetector;
class Scene3DContainer;
class CollisionBox;
class AnimatedSprite3D;
class HidableObject3DWrapper;


class CTFFlag
{
	CTFFlagState _state;
	TankData *_carrierData;
	std::shared_ptr<BitmapData> originalTexture;
	Scene3DContainer *container;
	HidableObject3DWrapper *hidabbleWrapper;

	void setPosition(Vector3 *value);
	void createSkin(int frameWidth, int frameHeight, void *materials);
	void reset();

public:

	static const double Z_DISPLACEMENT;
	BattleTeamType teamType;
	CollisionBox *triggerCollisionPrimitive;
	QString *_carrierId;
	Vector3 _basePosition;

	std::shared_ptr<AnimatedSprite3D> skin;
	//Vector3 skin;

	bool takeCommandSent;

	CTFFlag(BattleTeamType teamType, Vector3 *basePos, int frameWidth, int frameHeight, void *materials, std::shared_ptr<BitmapData> &originalTexture, TanksCollisionDetector *collisionDetector);
	~CTFFlag();
	TankData* getCarrierData() { return this->_carrierData; }
	CTFFlagState getState() { return this->_state; }
	QString getCarrierId();
	Vector3* getBasePosition() { return &this->_basePosition; }
	bool isAtBase();


public:
	void addToContainer(Scene3DContainer *container);
	void setCarrier(const QString &carrierId, TankData *carrierData);
	void returnToBase();
	void dropAt(Vector3 *dropPos, TanksCollisionDetector *collisionDetector);
	void dispose();
	void update(int time);
	void returnToNone();
};

#endif // CTFFLAG_H
