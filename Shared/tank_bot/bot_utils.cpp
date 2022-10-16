#include "bot_utils.h"
#include "alternativa/math/vector3.h"
#include "alternativa/math/matrix4.h"
#include "alternativa/math/quaternion.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/tanks/vehicles/tanks/tankskinhull.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "_global.h"



namespace BotUtils
{
	void calculate_gun_params(const TankSkin *skin,
							const Matrix4 *hullMatrix,
							const Vector3 *localBarrelOrigin,
							double turretDirection, //IN
							Vector3 *barrelOrigin, //OUT
							Vector3 *turretAxisX, //OUT
							Vector3 *turretAxisY) //OUT
	{
		_function_name("calculate_gun_params");
		const Vector3 &turretMountPoint = skin->hullDescriptor->turretMountPoint;

		Matrix4 turretMatrix;
		turretMatrix.setMatrix(turretMountPoint.x, turretMountPoint.y, turretMountPoint.z + 1, 0, 0, turretDirection);
		turretMatrix.append(hullMatrix);

		turretMatrix.transformVector(localBarrelOrigin, barrelOrigin);

		turretAxisX->x = turretMatrix.a;
		turretAxisX->y = turretMatrix.e;
		turretAxisX->z = turretMatrix.i;

		turretAxisY->x = turretMatrix.b;
		turretAxisY->y = turretMatrix.f;
		turretAxisY->z = turretMatrix.j;
	}

	void calculate_gun_params(TankSkin *skin,
							const Matrix4 *hullMatrix,
							double turretDirection, //IN
							Vector3 *turretAxisY) //OUT
	{
		const Vector3 &turretMountPoint = skin->hullDescriptor->turretMountPoint;

		Matrix4 turretMatrix;
		turretMatrix.setMatrix(turretMountPoint.x, turretMountPoint.y, turretMountPoint.z + 1, 0, 0, turretDirection);
		turretMatrix.append(hullMatrix);

		turretAxisY->x = turretMatrix.b;
		turretAxisY->y = turretMatrix.f;
	}


	double distance_d(const Vector3 *pos_a, const Vector3 *pos_b)
	{
		return sqrt(pow2(pos_a->x - pos_b->x) + pow2(pos_a->y - pos_b->y) + pow2(pos_a->z - pos_b->z));
	}

	void update_turret_transform(TankSkin *skin, const Matrix4 *hullMatrix)
	{
		const Vector3 &turretMountPoint = skin->hullDescriptor->turretMountPoint;

		Matrix4 turretMatrix;
		turretMatrix.setMatrix(turretMountPoint.x, turretMountPoint.y, turretMountPoint.z + 1, 0, 0, skin->turretDirection);
		turretMatrix.append(hullMatrix);

		Vector3 vector;
		turretMatrix.getEulerAngles(&vector);
		skin->turretMesh->position.x = turretMatrix.d;
		skin->turretMesh->position.y = turretMatrix.h;
		skin->turretMesh->position.z = turretMatrix.l;
		skin->turretMesh->rotation.copy(&vector);
	}


	double dir_step(int32_t local_x, int32_t local_y, int32_t target_x, int32_t target_y, double dir_center, double hull_direction)
	{
		double x, y;

		BotUtils::cords_to_dir(target_x, target_y, local_x, local_y, &x, &y);

		const double s = 300.;
		x *= s;
		y *= s;

#if 1
		const int32_t position_left_x = target_x + y;
		const int32_t position_left_y = target_y + -x;
		BotUtils::cords_to_dir(local_x, local_y, position_left_x, position_left_y, &x, &y);
		double dir_left = BotUtils::direction_normalize(atan2(y, x) - hull_direction);
		return BotUtils::difference(dir_left, dir_center) * 0.2; //left(5) from center ([/ 5] -> [* 0.2])
#else
		const double position_left_x = target_x + y;
		const double position_left_y = target_y + -x;
		const double position_right_x = target_x + -y;
		const double position_right_y = target_y + x;
		BotUtils::cords_to_dir(local_x, local_y, position_left_x, position_left_y, &x, &y);
		double dir_left = BotUtils::direction_normalize(atan2(y, x) - hull_direction);
		BotUtils::cords_to_dir(local_x, local_y, position_right_x, position_right_y, &x, &y);
		double dir_right = BotUtils::direction_normalize(atan2(y, x) - hull_direction);
		return BotUtils::difference(dir_left, dir_right) * 0.1; //left(5) + right(5) = 10 ([/ 10] -> [* 0.1])
#endif
	}
}
