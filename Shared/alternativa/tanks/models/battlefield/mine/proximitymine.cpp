#include "proximitymine.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/primitives/collisionsphere.h"
#include "gametime.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/math/matrix3.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "mymath.h"
#include "alternativa/object/clientobjectuser.h"
#include "MineModelData.h"


static const int FLASH_GROW = 1;
static const int FLASH_FADE = 2;
static const int FLASH_DONE = 3;


ProximityMine::ProximityMine(std::shared_ptr<Mesh> &referenceMesh)
{
	this->collisionPrimitive = new CollisionSphere(1, CollisionGroup::WEAPON);
	this->mesh = Mesh_clone(referenceMesh);
	this->next = nullptr;
	this->prev = nullptr;
	this->hitCommandSent = false;
	this->armed = false;
	this->mineModelData = nullptr;
	this->flashBaseTime = 0;
	this->flashState = 0;
}

ProximityMine::~ProximityMine()
{
	delete this->collisionPrimitive;
}

ProximityMine *ProximityMine::create(const QString &id, const QString &ownerId, double proximityRadius, std::shared_ptr<Mesh> &referenceMesh, Material *material, BattleTeamType teamType, MineModelData *mineModelData)
{
	/*
	if (pool == null)
	{
		mine = new ProximityMine(referenceMesh);
	}
	else
	{
		mine = pool;
		pool = pool.next;
		mine.next = null;
	}
	*/

	ProximityMine *mine = new ProximityMine(referenceMesh);
	mine->init(id, ownerId, proximityRadius, material, teamType, mineModelData);
	return mine;
}


void ProximityMine::dispose()
{
	delete this;
	/*
	this->id = null;
	this->ownerId = null;
	var _local_1:* = this->mesh;
	(_local_1.alternativa3d::removeFromParent());
	this->mineModelData = null;
	this->prev = null;
	this->next = ((pool == null) ? null : pool);
	pool = this;
	*/
}

void ProximityMine::arm()
{
	this->armed = true;
	this->flashBaseTime = GetGameTime();
	this->flashState = FLASH_GROW;
	//this->mesh.colorTransform = this->colorTransform;
	this->mesh->alpha = 1;
}

void ProximityMine::setPosition(const Vector3 *pos, const Vector3 *normal)
{
	this->position.copy(pos);
	this->normal.copy(normal);
	this->mesh->position.copy(pos);

	Vector3 vector;
	vector.cross2(&Vector3::Z_AXIS, normal);
	vector.normalize();

	double angle = MyMath::acos(normal->dot(&Vector3::Z_AXIS));
	Matrix3 matrix;
	matrix.fromAxisAngle(&vector, angle);
	matrix.getEulerAngles(&vector);
	this->mesh->rotation.copy(&vector);
	this->collisionPrimitive->transform.setPosition(pos);
	this->collisionPrimitive->calculateAABB();
}

void ProximityMine::addToContainer(Scene3DContainer *container)
{
	(void)container;
	//container.addChild(this->mesh);
}

bool ProximityMine::canExplode(TankData *tankData)
{
	return (((((this->teamType == BattleTeamType::NONE) || (!(this->teamType == tankData->teamType))) && (this->armed)) && (!(this->ownerId == tankData->user->getId()))) && (tankData->enabled));
}

void ProximityMine::update(int now, int deltaMillis, TankData *localUserData)
{
	(void)deltaMillis;

	if ((!(this->armed)))
	{
		//this->mesh->alpha = this->blinker.updateValue(now, deltaMillis);
	}
	else
	{
		if (this->flashState != FLASH_DONE)
		{
			this->updateFlash(now);
		}
		else
		{
			if (((localUserData == nullptr) || (localUserData->health == 0)))
			{
				this->mesh->visible = false;
			}
			else
			{
				if (this->canExplode(localUserData))
				{
					this->updateVisibility(localUserData);
				}
				else
				{
					this->mesh->visible = true;
				}
			}
		}
	}
}

void ProximityMine::init(const QString &id, const QString &ownerId, double proximityRadius, Material *material, BattleTeamType teamType, MineModelData *mineModelData)
{
	(void)material;

	this->id = id;
	this->ownerId = ownerId;
	this->setProximityRadius(proximityRadius);
	this->teamType = teamType;
	this->mineModelData = mineModelData;
	//this->mesh->colorTransform = nullptr;
	this->mesh->alpha = 1;
	this->mesh->visible = true;
	//this->mesh->setMaterialToAllFaces(material);
	this->armed = false;
	this->hitCommandSent = false;
	this->flashState = FLASH_DONE;
	this->flashBaseTime = GetGameTime();
	//this->blinker.init(this->flashBaseTime);
}

void ProximityMine::setProximityRadius(double value)
{
	this->collisionPrimitive->r = value;
	this->collisionPrimitive->calculateAABB();
}

void ProximityMine::updateVisibility(TankData *localUserData)
{
	this->mesh->visible = true;
	Vector3 vector;
	vector.diff(&localUserData->tank->state.pos, &this->position);
	double d = vector.length();
	if (d > this->mineModelData->farRadius)
	{
		this->mesh->visible = false;
	}
	else
	{
		if (d < this->mineModelData->nearRadius)
		{
			this->mesh->alpha = 1.;
		}
		else
		{
			this->mesh->alpha = (this->mineModelData->farRadius - d) / (this->mineModelData->farRadius - this->mineModelData->nearRadius);
		}
	}
}

void ProximityMine::updateFlash(int now)
{
	switch (this->flashState)
	{
		case FLASH_GROW:
			if (now < (this->flashBaseTime + this->mineModelData->armedFlashDuration))
			{
				//this->setColorOffset(((this->mineModelData->flashChannelOffset * (now - this->flashBaseTime)) / this->mineModelData->armedFlashDuration));
			}
			else
			{
				//this->setColorOffset(this->mineModelData->flashChannelOffset);
				this->flashBaseTime = (this->flashBaseTime + (this->mineModelData->armedFlashDuration + this->mineModelData->armedFlashFadeDuration));
				this->flashState = FLASH_FADE;
			}
			break;
		case FLASH_FADE:
			if (now < this->flashBaseTime)
			{
				//this->setColorOffset(((this->mineModelData->flashChannelOffset * (this->flashBaseTime - now)) / this->mineModelData->armedFlashFadeDuration));
			}
			else
			{
				//this->setColorOffset(0);
				this->flashState = FLASH_DONE;
				//this->mesh->colorTransform = null;
			}
			break;
	}
}
