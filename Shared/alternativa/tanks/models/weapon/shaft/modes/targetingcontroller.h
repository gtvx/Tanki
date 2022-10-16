#ifndef TARGETINGCONTROLLER_H
#define TARGETINGCONTROLLER_H

#include "../LinearInterpolator.h"

class TargetingCameraController;
class GameCamera;
class ShaftDeactivationTask;
class TankSkin;
enum class ShaftModes;
class ShaftModel;
class ShaftSFXModel;
class TankModel;
class BattlefieldModel;
class Vector3;
class Quaternion;
class Matrix4;


class TargetingController
{
	bool up;
	bool down;
	LinearInterpolator fovInterpolator;
	LinearInterpolator rangeInterpolator;
	int exitTime;
	bool fired;
	bool chargingEffectActive;
	int timeLeft;
	LinearInterpolator alphaInterpolator;
	LinearInterpolator reticleAlphaInterpolator;
	int stateDuration;
	double targetFOV;
	Vector3 *targetCameraPosition;
	Quaternion *initialCameraOrientation;
	Quaternion *targetCameraOrientation;
	Quaternion *cameraOrientation;
	Vector3 *initialCameraPosition;
	Vector3 *cameraPosition;
	Matrix4 *cameraMatrix;
	Matrix4 *turretMatrix;
	Vector3 *cameraAngles;
	BattlefieldModel *battlefieldModel;
	double initalEnegry;
	double lastEnergy;
	double savedTurnAcceleration;
	double savedTurnSpred;
	TankModel *tankModel;
	ShaftSFXModel *sfxModel;

public:

	TargetingCameraController* cameraController;
	ShaftModes shaftMode;
	GameCamera *camera;
	TankSkin *skin;
	ShaftDeactivationTask *deactivationTask;
	ShaftModel *shaftModel;

	TargetingController(ShaftModel *shaftModel);

	void enter(int time);
	void exit();


private:
	void setTurretSpeedFactor(double value);

public:
	void update(int param1, int param2);

private:
	void calculateCameraInitialValues();
	void updateCamera(double param1);
};

#endif // TARGETINGCONTROLLER_H
