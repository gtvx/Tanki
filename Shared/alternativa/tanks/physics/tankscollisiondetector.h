#ifndef TANKSCOLLISIONDETECTOR_H
#define TANKSCOLLISIONDETECTOR_H

#include "alternativa/physics/collision/ICollisionDetector.h"
#include "alternativa/physics/collision/MinMax.h"
#include "qglobal.h"
#include <memory>

class ICollider;
class CollisionKdTree;
class BodyList;
class CollisionKdNode;
class BoundBox;
class Body;


class TanksCollisionDetector : public ICollisionDetector
{
	ICollider *colliders[18];
	RayIntersection *_rayHit;
	RayIntersection *_dynamicIntersection;
	double threshold;
	BodyList *bodies;

public:
	CollisionKdTree *tree;

	TanksCollisionDetector();
	virtual ~TanksCollisionDetector();
	Contact* getAllContacts(Contact *contact);
	bool intersectRay(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result);
	bool intersectRayNecessarilyDynamic(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result);
	bool intersectRayWithDynamic(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result);
	bool intersectRayWithStatic(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result);
	bool getContact(CollisionPrimitive *prim1, CollisionPrimitive *prim2, Contact *contact);
	bool testCollision(CollisionPrimitive *prim1, CollisionPrimitive *prim2);
	void destroy();
	void addBody(Body *body);
	void removeBody(Body *body);
	void buildKdTree(std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> collisionPrimitives, BoundBox *boundBox);


	bool raycastStatic(const Vector3 *origin,
					   const Vector3 *dir,
					   int collisionGroup,
					   double maxTime,
					   RayIntersection *result);

	bool hasStaticHit(Vector3 *origin, Vector3 *dir, int collisionGroup, double maxTime);

private:
	void addCollider(int type1, int type2, ICollider *collider);
	Contact *getPrimitiveNodeCollisions(CollisionKdNode *node, CollisionPrimitive *primitive, Contact *contact);
	bool getRayBoundBoxIntersection(const Vector3 *origin, const Vector3 *dir, const BoundBox *bb, MinMax *time);
	bool testRayAgainstNode(CollisionKdNode *node, const Vector3 *origin, const Vector3 *localOrigin, const Vector3 *dir, int collisionGroup, double t1, double t2, IRayCollisionPredicate *predicate, RayIntersection *result);
	bool getRayNodeIntersection(const Vector3 *origin, const Vector3 *dir, int collisionGroup, QVector<CollisionPrimitive*> *primitives, QVector<int> *indices, IRayCollisionPredicate *predicate, RayIntersection *intersection);
};

#endif // TANKSCOLLISIONDETECTOR_H
