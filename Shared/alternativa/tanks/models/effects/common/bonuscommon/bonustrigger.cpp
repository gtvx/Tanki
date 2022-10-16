#include "bonustrigger.h"
#include "bonusconst.h"
#include "alternativa/math/vector3.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/physics/collision/primitives/collisionbox.h"
#include "alternativa/physics/collisionprimitivelistitem.h"
#include "alternativa/physics/collisionprimitivelist.h"
#include "alternativa/physics/body.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "parabonus2.h"



BonusTrigger::BonusTrigger(ParaBonus2 *bonus)
{
	this->battleRunner = nullptr;
	this->bonus = bonus;
	double bonusHalfSize = BonusConst::BONUS_HALF_SIZE;
	Vector3 size(bonusHalfSize, bonusHalfSize, bonusHalfSize);

	this->collisionBox = new CollisionBox(&size, CollisionGroup::BONUS_WITH_TANK);
}

BonusTrigger::~BonusTrigger()
{
	delete this->collisionBox;
}

void BonusTrigger::activate(BattlefieldModel *battleRunner)
{
	if (this->battleRunner == nullptr)
	{
		this->battleRunner = battleRunner;
		battleRunner->addTrigger(this);
	}
}

void BonusTrigger::deactivate()
{
	if (this->battleRunner != nullptr)
	{
		this->battleRunner->removeTrigger(this);
		this->battleRunner = nullptr;
	}
}

void BonusTrigger::update(double x, double y, double z, double rx, double ry, double rz)
{
	Matrix4 *transform = &this->collisionBox->transform;
	transform->setMatrix(x, y, z, rx, ry, rz);
	this->collisionBox->calculateAABB();
}

void BonusTrigger::setTransform(const Vector3 *position, const Matrix3 *m)
{
	this->collisionBox->transform.setFromMatrix3(m, position);
	this->collisionBox->calculateAABB();
}

void BonusTrigger::checkTrigger(Body *body)
{
	CollisionPrimitiveListItem *item = body->collisionPrimitives->head;
	while (item != nullptr)
	{
		CollisionPrimitive *primitive = item->primitive;
		if (this->battleRunner->bfData->collisionDetector->testCollision(primitive, this->collisionBox))
		{
			this->bonus->onTriggerActivated();
			return;
		}
		item = item->next;
	}
}
