#ifndef TANKDATA_H
#define TANKDATA_H

#include <memory>

class IWeapon;
class Tank;
enum class TankSpawnState;
enum class BattleTeamType;
class QString;
class ClientObjectUser;
class ClientObjectWeapon;
class ClientObjectHull;


class TankData
{
public:
	TankData();
	~TankData();

	static TankData *localTankData;

	ClientObjectUser *user;
	ClientObjectWeapon *turret;
	ClientObjectHull *hull;

	std::shared_ptr<ClientObjectUser> _user;
	std::shared_ptr<ClientObjectWeapon> _turret;
	std::shared_ptr<ClientObjectHull> _hull;

	//BitmapData *coloring;
	//BitmapData *deadColoring;
	bool local;
	bool enabled;

	Tank *tank;
	QString *userName;
	IWeapon *weapon;
	//TankSounds *sounds;

	int health;
	int ctrlBits;
	int incarnation;
	int deadTime;
	int userRank;
	int tankCollisionCount;

	BattleTeamType teamType;
	TankSpawnState spawnState;

	double mass;
	double power;

	//private var events:CircularObjectBuffer = new CircularObjectBuffer(EVENT_BUFFER_SIZE);
};

#endif // TANKDATA_H
