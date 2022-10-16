#ifndef FLYCAMERACONTROLLER_H
#define FLYCAMERACONTROLLER_H

#include "cameracontrollerbase.h"
#include "ICameraController.h"
#include "alternativa/math/vector3.h"

class AngleValues;


class FlyCameraController : public CameraControllerBase, public ICameraController
{
	Vector3 p1;
	Vector3 p2;
	Vector3 p3;
	Vector3 p4;

	//private var p1:Vector3 = new Vector3();
	//private var p2:Vector3 = new Vector3();
	//private var p3:Vector3 = new Vector3();
	//private var p4:Vector3 = new Vector3();
	double totalDistance;
	double distance;
	double acceleration;
	double speed;
	AngleValues *angleValuesX;
	AngleValues *angleValuesZ;

public:
	FlyCameraController(GameCamera *camera);
	virtual ~FlyCameraController();


	void init(const Vector3 *targetPosition, const Vector3 *targetAngles, int travelTime);
	void update(uint32_t time, uint32_t deltaMsec) override;

private:
	void bezier(double t, Vector3 *p1, Vector3 *p2, Vector3 *p3, Vector3 *p4, Vector3 *result);

};

#endif // FLYCAMERACONTROLLER_H
