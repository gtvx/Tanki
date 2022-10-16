#ifndef THUNDERSHOTEFFECT_H
#define THUNDERSHOTEFFECT_H

#include "alternativa/tanks/sfx/IGraphicEffect.h"
#include "alternativa/tanks/utils/objectpool/pooledobject.h"
#include "alternativa/math/vector3.h"
#include <memory>

class Object3D;
class Vector3;
class Material;
class Sprite3D;
class Mesh;
class Trail;


class ThunderShotEffect : public  IGraphicEffect,  public PooledObject
{
	Object3D *turret;
	Vector3 localMuzzlePoint;
	std::shared_ptr<Sprite3D> sprite1;
	std::shared_ptr<Sprite3D> sprite2;
	std::shared_ptr<Sprite3D> sprite3;
	std::shared_ptr<Trail> ray1;
	std::shared_ptr<Trail> ray2;
	std::shared_ptr<Trail> ray3;
	double distance1 = 40;
	double distance2 = 75;
	double distance3 = 80;
	double rayDistance1 = 0;
	double rayDistance2 = 0;
	double rayDistance3 = 0;
	double angle1;
	double angle2;
	double angle3;
	int timeToLive;

public:
	ThunderShotEffect(ObjectPool*);

	void init(Object3D *turret, const Vector3 *muzzleLocalPoint, std::shared_ptr<Material> material);
	void addToContainer(Scene3DContainer *container);
	bool play(int millis, GameCamera *camera);
	void destroy();
	void kill();
	ClientObjectUser* getOwner();
	void createParticles();

private:
	std::shared_ptr<Sprite3D> createSprite(double size);
	void setSpritePosition(Sprite3D *sprite, const Vector3 *basePoint, const Vector3 *dir, double distance);
	void setRayMatrix(Mesh *ray, double angle, const Vector3 *basePoint, Vector3 *dir, double distance, double dx, double dz);
};

#endif // THUNDERSHOTEFFECT_H
