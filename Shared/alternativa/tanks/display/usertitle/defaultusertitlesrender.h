#ifndef DEFAULTUSERTITLESRENDER_H
#define DEFAULTUSERTITLESRENDER_H

#include "UserTitlesRender.h"

class IBattleField;
class TankData;
class Vector3;
enum class BattleTeamType;
class TanksCollisionDetector;


class DefaultUserTitlesRender : public UserTitlesRender
{
	double titleShowDistance;
	double titleHideDistance;
	IBattleField *battlefield;

public:

	TankData *localUserData;
	DefaultUserTitlesRender();
	virtual ~DefaultUserTitlesRender();

	void updateTitle(TankData *tankData, const Vector3 *cameraPos) override;

private:
	bool isSameTeam(BattleTeamType teamType1, BattleTeamType teamType2);
	bool isTankInvisible(TankData *tankData, const Vector3 *cameraPos);
	bool isTankPointVisible(Vector3 *point, TankData *tankData, const Vector3 *cameraPos, TanksCollisionDetector *collisionDetector);

public:
	void configurateTitle(TankData *tankData) override;
	void setBattlefield(IBattleField *model) override;
	void setLocalData(TankData *model) override;
	void render() override;
};

#endif // DEFAULTUSERTITLESRENDER_H
