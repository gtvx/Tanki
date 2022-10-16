#ifndef CLIENTTANK_H
#define CLIENTTANK_H

#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/TankSpecification.h"
#include "com/alternativaplatform/projects/tanks/client/commons/types/TankState.h"
#include "TankSpawnState.h"

class ClientTank
{
public:
	bool self;
	BattleTeamType teamType;
	TankSpecification tankSpecification;
	TankState tankState;
	TankSpawnState spawnState;
	int incarnationId;
	int health;
};

#endif // CLIENTTANK_H
