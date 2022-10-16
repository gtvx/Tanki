#include "decalfactory.h"
#include "mymath.h"
#include "alternativa/math/vector3.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/tanks/physics/collisiongroup.h"
#include "alternativa/engine3d/containers/kdcontainer.h"
#include "alternativa/engine3d/materials/texturematerial.h"
//#include <QDebug>

/*
private static const ANGLE_LIMIT:Number = ((85 * Math.PI) / 180);//1.48352986419518
private static const rayHit:RayIntersection = new RayIntersection();
private static const direction:Vector3 = new Vector3();
private static const right:Vector3 = new Vector3();
private static const up:Vector3 = new Vector3();
private static const normal:Vector3 = new Vector3();
private static const origins:Vector.<Vector3> = Vector.<Vector3>([new Vector3(), new Vector3(), new Vector3(), new Vector3(), new Vector3()]);
private static const position3D:Vector3D = new Vector3D();
private static const normal3D:Vector3D = new Vector3D();
*/

static const double ANGLE_LIMIT = (85. * MyMath::PI) / 180.; //1.48352986419518


static double getRotation(RotationState rotationState)
{
	switch (rotationState)
	{
		case RotationState::WITHOUT_ROTATION:
			return 0;
		default:
			return 2; //(MyMath::random() * 2) * MyMath::PI;
	}
}

DecalFactory::DecalFactory(ICollisionDetector *collisionDetector)
{
	this->collisionDetector = collisionDetector;
}

std::shared_ptr<Decal> DecalFactory::createDecal(const Vector3 *hitPosition,
												 const Vector3 *muzzlePosition,
												 double radius,
												 TextureMaterial *material,
												 KDContainer *container,
												 RotationState rotationState)
{
	//var _loc8_:Vector3;

	Vector3 direction;

	direction.diff(hitPosition, muzzlePosition);

	double _loc7_ = direction.length() + 200;

	direction.normalize();

	Vector3 right, up;

	Vector3 origins[5];

	right.cross2(&direction, &Vector3::Z_AXIS);
	right.normalize();

	up.cross2(&right, &direction);

	origins[4].copy(muzzlePosition);

	origins[0].copy(muzzlePosition);
	origins[0].addScaled(50, &right);

	origins[1].copy(muzzlePosition);
	origins[1].addScaled(50, &up);

	origins[2].copy(muzzlePosition);
	origins[2].addScaled(-50, &right);

	origins[3].copy(muzzlePosition);
	origins[3].addScaled(-50, &up);

	Vector3 normal;
	normal.reset(0, 0, 0);


	for (int i = 0; i < 5; i++)
	{
		Vector3 *v = &origins[i];

		RayIntersection rayHit;

		if (this->collisionDetector->intersectRayWithStatic(v, &direction, CollisionGroup::STATIC, _loc7_, nullptr, &rayHit))
		{
			normal.add(&rayHit.normal);
		}
	}

	normal.normalize();
	double rotation = ::getRotation(rotationState);
	return container->createDecal(hitPosition, &normal, radius, rotation, ANGLE_LIMIT, 300, material);
}

