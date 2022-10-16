#ifndef SHAFTDEACTIVATIONTASK_H
#define SHAFTDEACTIVATIONTASK_H

#include "alternativa/tanks/models/battlefield/logic/LogicUnit.h"

class GameCamera;
class TankSkin;
class BattlefieldModel;


class ShaftDeactivationTask : public LogicUnit
{
	GameCamera *camera;
	TankSkin *skin;
	BattlefieldModel *battle;
	double targetFov;

public:
	ShaftDeactivationTask(GameCamera *camera);
	void setSkin(TankSkin *skin);
	void setTargetFov(double value);
	void start();
	void stop();
	void runLogic(int param1, int param2) override;
};

#endif // SHAFTDEACTIVATIONTASK_H
