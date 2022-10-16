#ifndef GAMEMODES_H
#define GAMEMODES_H

class IGameMode;
class QString;

class GameModes
{
public:
	static IGameMode* getGameMode(const QString &id);
	static bool isSpace(IGameMode *gameMode);
	static bool isDay(IGameMode *gameMode);
	static bool isHalloween(IGameMode *gameMode);
};

#endif // GAMEMODES_H
