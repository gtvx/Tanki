#ifndef BODY_H
#define BODY_H


#include "collision/types/boundbox.h"
#include "alternativa/math/matrix3.h"
#include "bodystate.h"
#include "alternativa/math/vector3.h"
#include "alternativa/physics/bodymaterial.h"
#include <memory>

class IBodyCollisionPredicate;
class BodyState;
class Vector3;
class BodyMaterial;
class PhysicsScene;
class CollisionPrimitiveList;
class Contact;
class CollisionPrimitive;
class Matrix4;



class Body
{
public:

	class T
	{
		const char *_name;
	public:
		T(const char *name) : _name(name) {}
		const char* getName() const { return this->_name; }
	};

	const T *type;

	static const T *TYPE;

	bool isType(const T *type)
	{
		return this->type == type;
	}

	Body(const T *type, double invMass, const Matrix3 *invInertia, bool localTank = false);
	virtual ~Body();
	virtual void destroy();
	static double linDamping;
	static double rotDamping;
	const int MAX_CONTACTS = 29;
	PhysicsScene *world;
	bool movable;
	bool canFreeze;
	bool frozen;
	int freezeCounter;
	int id;
	BoundBox aabb;
	IBodyCollisionPredicate *postCollisionPredicate;
	BodyState state;
	BodyState prevState;
	Vector3 accel;
	Vector3 angleAccel;
	BodyMaterial material;
	double invMass;
	Matrix3 invInertia;
	Matrix3 invInertiaWorld;
	Matrix3 baseMatrix;
	Contact *contacts[100];
	int contactsNum;
	CollisionPrimitiveList *collisionPrimitives;
	Vector3 forceAccum;
	Vector3 torqueAccum;
	virtual void beforePhysicsStep(double dt) = 0;
	void calcAccelerations();
	void saveState();
	void integrateVelocity(double dt);
	void integratePosition(double dt);
	void calcDerivedData();
	void restoreState();
	void applyRelPosWorldImpulse(const Vector3 *r, const Vector3 *dir, double magnitude);
	void clearAccumulators();
	void addCollisionPrimitive(CollisionPrimitive *primitive, const Matrix4 *localTransform = nullptr);
	void addWorldForce(const Vector3 *pos, const Vector3 *force);
	void interpolate(double t, Vector3 *pos, Quaternion *orientation) const;
	void addForce(const Vector3 *f);
	void addWorldForceScaled(const Vector3 *pos, const Vector3 *force, double scale);
};

#endif // BODY_H
