#include "defaultusertitlesrender.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/display/usertitle/usertitle.h"
#include "mymath.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"



DefaultUserTitlesRender::DefaultUserTitlesRender()
{
	this->titleShowDistance = 7000;
	this->titleHideDistance = 7050;
}

DefaultUserTitlesRender::~DefaultUserTitlesRender()
{

}


void DefaultUserTitlesRender::updateTitle(TankData *tankData, const Vector3 *cameraPos)
{

	Tank *tank = tankData->tank;
	if (tankData->health <= 0)
	{
		tankData->tank->title()->hide();
	}
	else
	{
		if (((!(tankData->local)) && (!(TankData::localTankData == nullptr))))
		{
			if ((!(this->isSameTeam(TankData::localTankData->teamType, tankData->teamType))))
			{
				const Vector3 *pos = &tank->state.pos;
				double dx = pos->x - cameraPos->x;
				double dy = pos->y - cameraPos->y;
				double dz = pos->z - cameraPos->z;
				double distance = MyMath::sqrt((((dx * dx) + (dy * dy)) + (dz * dz)));
				if (((distance >= this->titleHideDistance) || (this->isTankInvisible(tankData, cameraPos))))
				{
					tank->title()->hide();
				}
				else
				{
					if (distance < this->titleShowDistance)
					{
						tank->title()->show();
					}
				}
			}
			else
			{
				tank->title()->show();
			}
		}
	}
}

bool DefaultUserTitlesRender::isSameTeam(BattleTeamType teamType1, BattleTeamType teamType2)
{
	return ((!(teamType1 == BattleTeamType::NONE)) && (teamType1 == teamType2));
}


bool DefaultUserTitlesRender::isTankInvisible(TankData *tankData, const Vector3 *cameraPos)
{
	TanksCollisionDetector *collisionDetector = this->battlefield->getBattlefieldData()->collisionDetector;
	const Vector3 *points = tankData->tank->visibilityPoints;

	for (int i = 0; i < 6; i++)
	{
		Vector3 point;
		point.copy(&points[i]);
		if (this->isTankPointVisible(&point, tankData, cameraPos, collisionDetector))
		{
			return false;
		}
		i++;
	}

	return true;
}



bool DefaultUserTitlesRender::isTankPointVisible(Vector3 *point, TankData *tankData, const Vector3 *cameraPos, TanksCollisionDetector *collisionDetector)
{
	point->transformBy3(&tankData->tank->baseMatrix);
	point->add(&tankData->tank->state.pos);
	Vector3 rayVector, rayOrigin;
	rayOrigin.copy(cameraPos);
	rayVector.diff(point, &rayOrigin);
	RayIntersection rayIntersection;
	return !collisionDetector->intersectRayWithStatic(&rayOrigin, &rayVector, CollisionGroup::STATIC, 1, nullptr, &rayIntersection);
}

void DefaultUserTitlesRender::configurateTitle(TankData *tankData)
{
	if (TankData::localTankData == nullptr /*|| TankData::localTankData->teamType == nullptr*/)
	{
		return;
	}
	int configFlags = UserTitle::BIT_LABEL | UserTitle::BIT_EFFECTS;
	if (this->isSameTeam(tankData->teamType, TankData::localTankData->teamType))
	{
		configFlags = configFlags | UserTitle::BIT_HEALTH;
	}
	UserTitle *title = tankData->tank->title();
	title->setLabelText(*tankData->userName);
	title->setRank(tankData->userRank);
	title->setTeamType(tankData->teamType);
	title->setHealth(tankData->health);
	title->setConfiguration(configFlags);
}

void DefaultUserTitlesRender::setBattlefield(IBattleField *model)
{
	this->battlefield = model;
}

void DefaultUserTitlesRender::setLocalData(TankData *model)
{
	this->localUserData = model;
}

void DefaultUserTitlesRender::render()
{

}
