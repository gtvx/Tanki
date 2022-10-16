#include "planemuzzleflasheffect.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/math/vector3.h"
#include "simpleplane.h"
#include "alternativa/tanks/sfx/sfxutils.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include <QDebug>


PlaneMuzzleFlashEffect::PlaneMuzzleFlashEffect(ObjectPool *objectPool)
	: PooledObject(objectPool, 9)
{
	this->plane = new_SimplePlane(1, 1, 0.5, 0);
	this->plane->setUVs(0, 0, 0, 1, 1, 1, 1, 0);
	this->plane->shadowMapAlphaThreshold = 2;
	this->plane->depthMapAlphaThreshold = 2;
	this->plane->useShadowMap = false;
	this->plane->useLight = false;
}

void PlaneMuzzleFlashEffect::init(const Vector3 *pos, Object3D *turret, TextureMaterial *material, int timetoLive, double width, double length)
{
	this->localMuzzlePosition.copy(pos);
	this->turret = turret;
	this->timetoLive = timetoLive;
	this->plane->setMaterialToAllFaces(material);
	this->plane->setWidth(width);
	this->plane->setLength(length);
}

bool PlaneMuzzleFlashEffect::play(int millis, GameCamera *gameCamera)
{
	if (this->timetoLive < 0)
	{
		return false;
	}

	this->timetoLive -= millis;

	Matrix4 turretMatrix;
	Vector3 gunDirection, globalMuzzlePosition;

	turretMatrix.setMatrix(this->turret->position.x,
						   this->turret->position.y,
						   this->turret->position.z,
						   this->turret->rotation.x,
						   this->turret->rotation.y,
						   this->turret->rotation.z);

	turretMatrix.transformVector(&this->localMuzzlePosition, &globalMuzzlePosition);
	turretMatrix.getAxis(1, &gunDirection);
	SFXUtils::alignObjectPlaneToView(this->plane.get(), &globalMuzzlePosition, &gunDirection, &gameCamera->pos);
	return true;
}

void PlaneMuzzleFlashEffect::destroy()
{
	this->container->removeChild(this->plane.get());
	//this->plane = nullptr;
	this->container = nullptr;
	this->turret = nullptr;
	this->storeInPool();
}

void PlaneMuzzleFlashEffect::kill()
{
	this->timetoLive = -1;
}

void PlaneMuzzleFlashEffect::addToContainer(Scene3DContainer *container)
{
	this->container = container;
	container->addChild(this->plane.get());
}

ClientObjectUser* PlaneMuzzleFlashEffect::getOwner()
{
	return nullptr;
}
