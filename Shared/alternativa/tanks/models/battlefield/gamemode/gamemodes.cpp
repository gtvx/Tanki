#include "gamemodes.h"
#include "defaultgamemodel.h"
#include "nightgamemode.h"
#include "spacegamemode.h"
#include "newyeargamemode.h"
#include "halloweengamemode.h"
#include "daygamemode.h"
#include "foggamemode.h"
#include <QString>


//static const IGameMode *DEFAULT = new DefaultGameModel();
//static const NIGHT:IGameMode = new NightGameMode();
//static const SPACE:IGameMode = new SpaceGameMode();
//static const NEWYEAR:IGameMode = new NewYearGameMode();
//static const HALLOWEEN:IGameMode = new HalloweenGameMode();
//static const DAY:IGameMode = new DayGameMode();
//static const FOG:IGameMode = new FogGameMode();



namespace
{
	IGameMode *DEFAULT;
	IGameMode *NIGHT; //Ночь
	IGameMode *SPACE; //Космос
	IGameMode *NEWYEAR; //Новый год
	IGameMode *HALLOWEEN; //Хэллоуин
	IGameMode *DAY; //День
	IGameMode *FOG;  //Туман


	IGameMode* getDefault()
	{
		if (DEFAULT == nullptr)
			DEFAULT = new DefaultGameModel();
		return DEFAULT;
	}

	IGameMode* getNight()
	{
		if (NIGHT == nullptr)
			NIGHT = new NightGameMode();
		return NIGHT;
	}

	IGameMode* getSpace()
	{
		if (SPACE == nullptr)
			SPACE = new SpaceGameMode();
		return SPACE;
	}

	IGameMode* getNewYear()
	{
		if (NEWYEAR == nullptr)
			NEWYEAR = new NewYearGameMode();
		return NEWYEAR;
	}

	IGameMode* getHalloween()
	{
		if (HALLOWEEN == nullptr)
			HALLOWEEN = new HalloweenGameMode();
		return HALLOWEEN;
	}

	IGameMode* getDAY()
	{
		if (DAY == nullptr)
			DAY = new DayGameMode();
		return DAY;
	}

	IGameMode* getFog()
	{
		if (FOG == nullptr)
			FOG = new FogGameMode();
		return FOG;
	}


}


IGameMode* GameModes::getGameMode(const QString &id)
{
	//return getFog();
	//return getHalloween();
	//return getNight();
	//return getDAY();
	//return getDefault();
	//return getNewYear();

	if (id == "night")
		return getNight();

	if (id == "space")
		return getSpace();

	if (id == "ny")
		return getNewYear();

	if (id == "halloween")
		return getHalloween();

	if (id == "day")
		return getDAY();

	if (id == "fog")
		return getFog();

	return getDefault();
}

bool GameModes::isSpace(IGameMode *gameMode)
{
	return gameMode == SPACE;
}

bool GameModes::isDay(IGameMode *gameMode)
{
	return gameMode == DAY;
}

bool GameModes::isHalloween(IGameMode *gameMode)
{
	return gameMode == HALLOWEEN;
}
