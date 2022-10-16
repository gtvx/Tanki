#include "dominationbeameffect.h"
#include "animatedbeam.h"
#include "BeamProperties.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/tanks/sfx/sfxutils.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"


//private static const conZOffset:ConsoleVarFloat = new ConsoleVarFloat("beam_zoffset", 100, 0, 1000);

static const double conZOffset = 100;



DominationBeamEffect::DominationBeamEffect(ObjectPool *pool) :
	PooledObject(pool, 0)
{
	 this->beam = new_AnimatedBeam(1, 1, 1, 0);
}

void DominationBeamEffect::init(Object3D *object, const Vector3 *pos, const BeamProperties *beamProperties, Dictionary *excludedObects)
{
	this->object = object;
	this->endPosition.copy(pos);
	this->beam->setMaterials(beamProperties->beamTipMaterial.get(), beamProperties->beamMaterial.get());
	this->beam->setUnitLength(beamProperties->unitLength);
	this->beam->animationSpeed = beamProperties->animationSpeed;
	this->setBeamWidth(beamProperties->beamWidth);
	this->beam->setURange(beamProperties->uRange);
	this->beam->alpha = beamProperties->alpha;
	this->alive = true;
	this->excludedObects = excludedObects;
}

bool DominationBeamEffect::play(int millis, GameCamera *camera)
{
	Vector3 startPosition;
	startPosition.x = this->object->position.x;
	startPosition.y = this->object->position.y;
	startPosition.z = this->object->position.z + conZOffset;
	Vector3 direction;
	direction.diff(&this->endPosition, &startPosition);
	this->beam->setLength(direction.length());
	direction.normalize();
	SFXUtils::alignObjectPlaneToView(this->beam.get(), &startPosition, &direction, &camera->pos);
	this->beam->update(millis * 0.001);
	return this->alive;
}

void DominationBeamEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->beam.get());
	//this->excludedObects[this->beam] = true;
}

void DominationBeamEffect::destroy()
{
	this->object = nullptr;
	this->container->removeChild(this->beam.get());
	this->beam->setMaterialToAllFaces(nullptr);
	//delete this->excludedObects[this->beam];
	//this->excludedObects = nullptr;
	this->storeInPool();
}

void DominationBeamEffect::kill()
{
	this->alive = false;
}

ClientObjectUser *DominationBeamEffect::getOwner()
{
	return nullptr;
}

void DominationBeamEffect::setBeamWidth(double value)
{
	this->beam->setWidth(value);
	this->beam->setTipLength(value);
}
