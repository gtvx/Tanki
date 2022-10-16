#ifndef BOT_UTILS_H
#define BOT_UTILS_H

#include <math.h>
#include "stdint.h"
#include "mymath.h"


class TankSkin;
class Matrix4;
class Vector3;


namespace BotUtils
{
	void calculate_gun_params(const TankSkin *skin,
							const Matrix4 *hullMatrix,
							const Vector3 *localBarrelOrigin,
							double turretDirection, //IN
							Vector3 *barrelOrigin, //OUT
							Vector3 *turretAxisX, //OUT
							Vector3 *turretAxisY); //OUT

	void calculate_gun_params(TankSkin *skin,
							const Matrix4 *hullMatrix,
							double turretDirection, //IN
							Vector3 *turretAxisY); //OUT


	static inline double difference(const double a, const double b)
	{
		if (a < b) {
			double d = b - a;
			if (d > M_PI) {
				d = d - MyMath::PI2;
			}
			return -d;
		} else if (a > b) {
			double d = a - b;
			if (d > M_PI) {
				d = MyMath::PI2 - d;
				d = -d;
			}
			return d;
		}
		return 0.;
	}


	static inline void cords_to_dir(int32_t ax, int32_t ay, int32_t bx, int32_t by, double *out_x, double *out_y)
	{
		int32_t x = bx - ax;
		int32_t y = by - ay;
		uint32_t d = (x * x) + (y * y);
		if (d == 0)
		{
			*out_x = 1.;
			*out_y = 0.;
		}
		else
		{
			double v = 1. / sqrt(d);
			*out_x = x * v;
			*out_y = y * v;
		}
	}

	void update_turret_transform(TankSkin *skin, const Matrix4 *hullMatrix);

	static inline uint32_t int_xyz_to_distance_plain(int32_t ax, int32_t ay, int32_t az, int32_t bx, int32_t by, int32_t bz)
	{
		return abs(ax - bx) + abs(ay - by) + abs(az - bz);
	}

	static inline uint32_t int_xy_to_distance_plain(int32_t ax, int32_t ay, int32_t bx, int32_t by)
	{
		return abs(ax - bx) + abs(ay - by);
	}

	static inline uint32_t int_z_to_distance_plain(int32_t az, int32_t bz)
	{
		return abs(az - bz);
	}


	static inline double direction_normalize(double direction)
	{
		if (direction < -M_PI) {
			direction += MyMath::PI2;
		} else if (direction > M_PI) {
			direction -= MyMath::PI2;
		}
		return direction;
	}

	double distance_d(const Vector3 *pos_a, const Vector3 *pos_b);

	static inline double pow2(double value)
	{
		return value * value;
	}

	static inline int32_t pow2(int32_t value)
	{
		return value * value;
	}

	static inline double distance_d_x_y(double ax, double ay, double bx, double by)
	{
		return std::sqrt(pow2(ax - bx) + pow2(ay - by));
	}

	static inline double distance_i_x_y(int32_t ax, int32_t ay, int32_t bx, int32_t by)
	{
		return std::sqrt(pow2(ax - bx) + pow2(ay - by));
	}

	static inline double distance_d_x_y_z(const double ax, double ay, double az, double bx, const double by, const double bz)
	{
		return std::sqrt(pow2(ax - bx) + pow2(ay - by) + pow2(az - bz));
	}

	double dir_step(int32_t local_x, int32_t local_y, int32_t target_x, int32_t target_y, double dir_center, double hull_direction);

	static inline double get_radian(int32_t ax, int32_t ay, int32_t bx, int32_t by)
	{
		double x = bx - ax;
		double y = by - ay;

		if(x == 0.)
			return (y > 0.) ? 3.141592653589793 : 0.;

		double a = std::atan(y / x);;
		a = (x > 0) ? a + 1.5707963267948966 : a + 4.71238898038469;
		return a;
	}
}

#endif // BOT_UTILS_H
