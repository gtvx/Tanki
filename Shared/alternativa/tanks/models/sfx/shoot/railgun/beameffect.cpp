#include "beameffect.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/tanks/models/sfx/shoot/railgun/shottrail.h"
#include "alternativa/tanks/sfx/sfxutils.h"
#include "mymath.h"
#include "alternativa/tanks/camera/gamecamera.h"


BeamEffect::BeamEffect(ObjectPool *bjectPool) :
	PooledObject(bjectPool, 0)
{
	startPoint = new Vector3();
	direction = new Vector3();
	this->trail = new_ShotTrail();
}

void BeamEffect::init(const Vector3 *param1, const Vector3 *param2, std::shared_ptr<Material> &material, double param4, double beginScale, double endScale, double moveDistance, int lifeTime)
{
	this->startPoint->copy(param1);
	this->direction->diff(param2, param1);
	double _loc9_ = this->direction->length();
	this->direction->scale(1. / _loc9_);
	this->beginScale = beginScale;
	this->endScale = endScale;
	this->moveDistance = moveDistance;
	this->lifeTime = lifeTime;
	this->trail->init(param4, _loc9_, material, moveDistance);
	this->time = 0;
}

bool BeamEffect::play(int millis, GameCamera *camera)
{
	if (this->time > this->lifeTime)
		return false;

	SFXUtils::alignObjectPlaneToView(this->trail.get(), this->startPoint, this->direction, &camera->pos);
	double _loc3_ = (double)this->time / (double)this->lifeTime;
	double _loc4_ = MyMath::sqrt(_loc3_);
	this->trail->scale.x = (this->beginScale + ((this->endScale - this->beginScale) * _loc4_));
	this->trail->alpha = 1 - _loc3_;
	this->trail->update(_loc4_);
	this->time += millis;
	return true;
}

void BeamEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->trail.get());
}

void BeamEffect::destroy()
{
	//this->trail->clear();
	this->container->removeChild(this->trail.get());
	//this->trail->destroy();
	this->container = nullptr;
	this->storeInPool();
}

void BeamEffect::kill()
{
	this->time = this->lifeTime + 1;
}

ClientObjectUser* BeamEffect::getOwner()
{
	return nullptr;
}
