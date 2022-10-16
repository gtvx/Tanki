#include "thundershoteffect.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "randomgenerator.h"
#include "mymath.h"
#include "alternativa/tanks/models/sfx/shoot/thunder/trail.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "alternativa/engine3d/materials/fillmaterial.h"
#include "alternativa/math/matrix3.h"


static const double speed1 = 500;
static const double speed2 = 1000;
static const double speed3 = 1500;
static const double raySpeed1 = 1500;
static const double raySpeed2 = 2500;
static const double raySpeed3 = 1300;
static std::shared_ptr<FillMaterial> rayMaterial;


ThunderShotEffect::ThunderShotEffect(ObjectPool *objectPool) :
	PooledObject(objectPool, 10)
{
	 this->createParticles();
}

void ThunderShotEffect::init(Object3D *turret, const Vector3 *muzzleLocalPoint, std::shared_ptr<Material> material)
{
	this->turret = turret;
	//this->localMuzzlePoint = muzzleLocalPoint;
	this->localMuzzlePoint.copy(muzzleLocalPoint);
	this->sprite1->material = material;
	this->sprite2->material = material;
	this->sprite3->material = material;
	this->timeToLive = 50;
	this->distance1 = 40;
	this->distance2 = 75;
	this->distance3 = 80;
	this->rayDistance1 = 0;
	this->rayDistance2 = 0;
	this->rayDistance3 = 0;
	this->angle1 = (RandomGenerator::random() * 2) * MyMath::PI;
	this->angle2 = (RandomGenerator::random() * 2) * MyMath::PI;
	this->angle3 = (RandomGenerator::random() * 2) * MyMath::PI;
}

void ThunderShotEffect::addToContainer(Scene3DContainer *container)
{
	container->addChild(this->sprite1.get());
	container->addChild(this->sprite2.get());
	container->addChild(this->sprite3.get());
	container->addChild(this->ray1.get());
	container->addChild(this->ray2.get());
	container->addChild(this->ray3.get());
}

bool ThunderShotEffect::play(int millis, GameCamera *camera)
{
	(void)camera;

	if (this->timeToLive < 0)
	{
		return false;
	}

	Matrix4 turretMatrix;

	turretMatrix.setMatrix(this->turret->position.x,
						   this->turret->position.y,
						   this->turret->position.z,
						   this->turret->rotation.x,
						   this->turret->rotation.y,
						   this->turret->rotation.z);

	Vector3 basePoint;
	turretMatrix.transformVector(&this->localMuzzlePoint, &basePoint);
	Vector3 direction;
	direction.x = turretMatrix.b;
	direction.y = turretMatrix.f;
	direction.z = turretMatrix.j;
	double dt = 0.001 * millis;
	this->rayDistance1 = (this->rayDistance1 + (dt * raySpeed1));
	this->rayDistance2 = (this->rayDistance2 + (dt * raySpeed2));
	this->rayDistance3 = (this->rayDistance3 + (dt * raySpeed3));
	this->setSpritePosition(this->sprite1.get(), &basePoint, &direction, this->distance1);
	this->setSpritePosition(this->sprite2.get(), &basePoint, &direction, this->distance2);
	this->setSpritePosition(this->sprite3.get(), &basePoint, &direction, this->distance3);
	this->setRayMatrix(this->ray1.get(), this->angle1, &basePoint, &direction, this->rayDistance1, 0, 10);
	this->setRayMatrix(this->ray2.get(), this->angle2, &basePoint, &direction, this->rayDistance2, -7, 0);
	this->setRayMatrix(this->ray3.get(), this->angle3, &basePoint, &direction, this->rayDistance3, 7, 0);
	this->distance1 = (this->distance1 + (dt * speed1));
	this->distance2 = (this->distance2 + (dt * speed2));
	this->distance3 = (this->distance3 + (dt * speed3));
	this->timeToLive = (this->timeToLive - millis);
	return true;
}

void ThunderShotEffect::destroy()
{
	this->sprite1->removeFromParent();
	this->sprite2->removeFromParent();
	this->sprite3->removeFromParent();
	this->ray1->removeFromParent();
	this->ray2->removeFromParent();
	this->ray3->removeFromParent();
	this->storeInPool();
}

void ThunderShotEffect::kill()
{
	this->timeToLive = -1;
}

ClientObjectUser *ThunderShotEffect::getOwner()
{
	return nullptr;
}

void ThunderShotEffect::createParticles()
{
	this->sprite1 = this->createSprite(120);
	this->sprite2 = this->createSprite(99.75);
	this->sprite3 = this->createSprite(79.5);

	if (rayMaterial == nullptr)
		rayMaterial = new_FillMaterial(4294753806);

	this->ray1 = new_Trail(0.8, rayMaterial);
	this->ray2 = new_Trail(0.75, rayMaterial);
	this->ray3 = new_Trail(0.82, rayMaterial);
}

std::shared_ptr<Sprite3D> ThunderShotEffect::createSprite(double size)
{
	std::shared_ptr<Sprite3D> sprite = new_Sprite3D(size, size, nullptr);
	sprite->rotation = (2 * MyMath::PI) * RandomGenerator::random();
	sprite->blendMode = BlendMode::SCREEN;
	return sprite;
}

void ThunderShotEffect::setSpritePosition(Sprite3D *sprite, const Vector3 *basePoint, const Vector3 *dir, double distance)
{
	sprite->position.x = basePoint->x + (dir->x * distance);
	sprite->position.y = basePoint->y + (dir->y * distance);
	sprite->position.z = basePoint->z + (dir->z * distance);
}

void ThunderShotEffect::setRayMatrix(Mesh *ray, double angle, const Vector3 *basePoint, Vector3 *dir, double distance, double dx, double dz)
{
	Vector3 axis;
	Matrix3 trailMatrix;
	trailMatrix.fromAxisAngle(&Vector3::Y_AXIS, angle);
	if (((dir->y < -0.99999) || (dir->y > 0.99999)))
	{
		axis.x = 0;
		axis.y = 0;
		axis.z = 1;
		angle = ((dir->y < 0) ? MyMath::PI : 0);
	}
	else
	{
		axis.x = dir->z;
		axis.y = 0;
		axis.z = -dir->x;
		axis.normalize();
		angle = MyMath::acos(dir->y);
	}
	Matrix3 trailMatrix2;
	trailMatrix2.fromAxisAngle(&axis, angle);
	trailMatrix.append(&trailMatrix2);
	Vector3 eulerAngles;
	trailMatrix.getEulerAngles(&eulerAngles);
	ray->rotation.x = eulerAngles.x;
	ray->rotation.y = eulerAngles.y;
	ray->rotation.z = eulerAngles.z;
	ray->position.x = (((basePoint->x + (dir->x * distance)) + (dx * trailMatrix.a)) + (dz * trailMatrix.c));
	ray->position.y = (((basePoint->y + (dir->y * distance)) + (dx * trailMatrix.e)) + (dz * trailMatrix.g));
	ray->position.z = (((basePoint->z + (dir->z * distance)) + (dx * trailMatrix.i)) + (dz * trailMatrix.k));
}
