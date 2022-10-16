#ifndef ICTFMODEL_H
#define ICTFMODEL_H

class TankData;
class Vector3;
enum class BattleTeamType;

class ICTFModel
{
public:
	virtual bool isPositionInFlagProximity(Vector3*, double, BattleTeamType) = 0;
	virtual bool isFlagCarrier(TankData*) = 0;
};

#endif // ICTFMODEL_H
