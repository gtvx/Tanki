#include "tankdata.h"
#include "_global.h"
#include <QString>

TankData *TankData::localTankData;


TankData::TankData()
{
	_function_name("TankData::TankData");
	mass = 1;
	power = 0;
	userName = new QString();
	tank = nullptr;
	user = nullptr;
	turret = nullptr;
	hull = nullptr;
	weapon = nullptr;
	enabled = false;
	ctrlBits = 0;
	local = false;
	health = 0;
	userRank = 1;
	tankCollisionCount = 0;
	incarnation = 0;
	deadTime = 0;
}

TankData::~TankData()
{
	_function_name("TankData::~TankData");
	delete userName;
}
