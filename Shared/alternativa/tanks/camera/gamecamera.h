#ifndef GAMECAMERA_H
#define GAMECAMERA_H

class Vector3;

#include "alternativa/engine3d/core/camera3d.h"

class GameCamera : public Camera3D
{

public:

	static const T *TYPE;
	const T *__type_next;

	GameCamera(const TT &t);

	Vector3 pos;
	Vector3 xAxis;
	Vector3 yAxis;
	Vector3 zAxis;

	void calculateAdditionalData();
	void getGlobalVector(const Vector3*, Vector3*);
	void updateFov();

	void clonePropertiesFromGameCamera(GameCamera* src);
};

std::shared_ptr<GameCamera> new_GameCamera();
std::shared_ptr<GameCamera> GameCamera_clone(const std::shared_ptr<GameCamera> &in);

#endif // GAMECAMERA_H
