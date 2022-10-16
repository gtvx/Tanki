#include "tankscollisiondetector.h"
#include "alternativa/physics/collision/collisionkdtree.h"
#include "alternativa/physics/collision/types/rayintersection.h"
#include "alternativa/physics/bodylist.h"
#include "alternativa/physics/body.h"
#include "alternativa/physics/bodylistitem.h"
#include "alternativa/physics/collision/collisionprimitive.h"
#include "alternativa/physics/collision/colliders/boxboxcollider.h"
#include "alternativa/physics/collision/colliders/boxspherecollider.h"
#include "alternativa/physics/collision/colliders/boxrectcollider.h"
#include "alternativa/physics/collision/colliders/boxtrianglecollider.h"
#include "alternativa/physics/collisionprimitivelistitem.h"
#include "alternativa/physics/collisionprimitivelist.h"
#include "alternativa/physics/collision/types/boundbox.h"
#include "alternativa/physics/contact.h"
#include "alternativa/physics/collision/IBodyCollisionPredicate.h"
#include "alternativa/physics/collision/IRayCollisionPredicate.h"
#include "alternativa/physics/collision/collisionkdnode.h"
#include "alternativa/physics/collision/collisionkdtree2d.h"
#include "alternativa/physics/contactpoint.h"
#include "alternativa/physics/collision/ICollisionPredicate.h"
#include "_global.h"
#include <QVector>
#include <QHash>


static ICollider* getCollider(ICollider **colliders, CollisionPrimitive *prim1, CollisionPrimitive *prim2)
{
	return colliders[prim1->type | prim2->type];
}


TanksCollisionDetector::TanksCollisionDetector()
{
	_function_name("TanksCollisionDetector::TanksCollisionDetector");

	for (int i = 0; i < 18; i++)
		colliders[i] = nullptr;

	threshold = 0.0001;

	_rayHit = new RayIntersection();
	_dynamicIntersection = new RayIntersection();

	this->tree = new CollisionKdTree();
	this->bodies = new BodyList();
	this->addCollider(CollisionPrimitive::BOX, CollisionPrimitive::BOX, new BoxBoxCollider());
	this->addCollider(CollisionPrimitive::BOX, CollisionPrimitive::RECT, new BoxRectCollider());
	this->addCollider(CollisionPrimitive::BOX, CollisionPrimitive::TRIANGLE, new BoxTriangleCollider());
	this->addCollider(CollisionPrimitive::BOX, CollisionPrimitive::SPHERE, new BoxSphereCollider());
}

TanksCollisionDetector::~TanksCollisionDetector()
{
	delete _rayHit;
	delete _dynamicIntersection;
	delete bodies;
	BodyListItem::clearPool();
	for (int i = 0; i < 18; i++) {
		ICollider *collider = colliders[i];
		if (collider != nullptr)
			collider->destroy();
	}
	delete tree;
}


void TanksCollisionDetector::addCollider(int type1, int type2, ICollider *collider)
{
	this->colliders[type1 | type2] = collider;
	//int k = (type1 <= type2) ? ((type1 << 16) | type2) : ((type2 << 16) | type1);
	//this->colliders->insert(k, collider);
}

Contact* TanksCollisionDetector::getAllContacts(Contact *contact)
{
	_function_name("TanksCollisionDetector::getAllContacts");

	BodyListItem *bodyListItem1 = this->bodies->head;
	while (bodyListItem1 != nullptr)
	{
		Body *body = bodyListItem1->body;
		if (!body->frozen)
		{
			CollisionPrimitiveListItem *cpListItem1 = body->collisionPrimitives->head;
			while (cpListItem1 != nullptr)
			{
				contact = this->getPrimitiveNodeCollisions(this->tree->rootNode, cpListItem1->primitive, contact);
				cpListItem1 = cpListItem1->next;
			}
		}

		BodyListItem *bodyListItem2 = bodyListItem1->next;
		while (bodyListItem2 != nullptr)
		{
			Body *otherBody = bodyListItem2->body;
			if ((body->frozen && otherBody->frozen) || !body->aabb.intersects(&otherBody->aabb, 0.1))
			{
				bodyListItem2 = bodyListItem2->next;
			}
			else
			{
				CollisionPrimitiveListItem *cpListItem1 = body->collisionPrimitives->head;
				bool abort = false;
				while (cpListItem1 != nullptr && !abort)
				{
					CollisionPrimitive *primitive1 = cpListItem1->primitive;
					CollisionPrimitiveListItem *cpListItem2 = otherBody->collisionPrimitives->head;
					while (cpListItem2 != nullptr)
					{
						if (this->getContact(primitive1, cpListItem2->primitive, contact))
						{
							if ((body->postCollisionPredicate != nullptr && !body->postCollisionPredicate->considerBodies(body, otherBody)) || (otherBody->postCollisionPredicate != nullptr && !otherBody->postCollisionPredicate->considerBodies(otherBody, body)))
							{
								abort = true;
								break;
							}
							contact = contact->next;
						}
						cpListItem2 = cpListItem2->next;
					}
					cpListItem1 = cpListItem1->next;
				}
				bodyListItem2 = bodyListItem2->next;
			}
		}
		bodyListItem1 = bodyListItem1->next;
	}
	return contact;
}

bool TanksCollisionDetector::intersectRay(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result)
{
	_function_name("TanksCollisionDetector::intersectRay");

	bool hasStaticIntersection = this->intersectRayWithStatic(origin, dir, collisionGroup, maxTime, predicate, result);
	bool hasDynamicIntersection = this->intersectRayWithDynamic(origin, dir, collisionGroup, maxTime, predicate, this->_dynamicIntersection);

	if (!(hasDynamicIntersection || hasStaticIntersection))
		return false;

	if (hasDynamicIntersection && hasStaticIntersection)
	{
		if (result->t > this->_dynamicIntersection->t)
		{
			result->copy(this->_dynamicIntersection);
		}
		return true;
	}

	if (hasStaticIntersection)
		return true;

	result->copy(this->_dynamicIntersection);
	return true;
}


bool TanksCollisionDetector::intersectRayNecessarilyDynamic(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result)
{
	bool hasDynamicIntersection = this->intersectRayWithDynamic(origin, dir, collisionGroup, maxTime, predicate, this->_dynamicIntersection);
	if (hasDynamicIntersection == false)
		return false;

	if (this->intersectRayWithStatic(origin, dir, collisionGroup, maxTime, predicate, result)) {
		if (result->t > this->_dynamicIntersection->t)
			result->copy(this->_dynamicIntersection);
	} else {
		result->copy(this->_dynamicIntersection);
	}

	return true;
}




bool TanksCollisionDetector::intersectRayWithStatic(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result)
{
	_function_name("TanksCollisionDetector::intersectRayWithStatic");

	MinMax time;

	if (!this->getRayBoundBoxIntersection(origin, dir, &this->tree->rootNode->boundBox, &time))
		return false;

	if (time.max < 0. || time.min > maxTime)
	{
		return false;
	}

	Vector3 _o;
	if (time.min <= 0)
	{
		time.min = 0;
		_o.x = origin->x;
		_o.y = origin->y;
		_o.z = origin->z;
	}
	else
	{
		_o.x = origin->x + (time.min * dir->x);
		_o.y = origin->y + (time.min * dir->y);
		_o.z = origin->z + (time.min * dir->z);
	}

	if (time.max > maxTime)
		time.max = maxTime;

	bool hasIntersection = this->testRayAgainstNode(this->tree->rootNode, origin, &_o, dir, collisionGroup, time.min, time.max, predicate, result);
	return hasIntersection ? (result->t <= maxTime) : false;
}

bool TanksCollisionDetector::getContact(CollisionPrimitive *prim1, CollisionPrimitive *prim2, Contact *contact)
{
	_function_name("TanksCollisionDetector::getContact");

	if ((prim1->collisionGroup & prim2->collisionGroup) == 0)
	{
		return false;
	}

	if (prim1->body != nullptr && prim1->body == prim2->body)
	{
		return false;
	}

	if (!prim1->aabb.intersects(&prim2->aabb, 0.01))
	{
		return false;
	}

	ICollider *collider = getCollider(this->colliders, prim1, prim2);


	if (!collider->getContact(prim1, prim2, contact)) {
		return false;
	}

	if (prim1->postCollisionPredicate != nullptr && !prim1->postCollisionPredicate->considerCollision(prim2))
	{
		return false;
	}

	if (prim2->postCollisionPredicate != nullptr && !prim2->postCollisionPredicate->considerCollision(prim1))
	{
		return false;
	}


	if (prim1->body != nullptr)
	{
		int &value = prim1->body->contactsNum;
		if (value < 100)
			prim1->body->contacts[value++] = contact;
	}

	if (prim2->body != nullptr)
	{
		int &value = prim2->body->contactsNum;
		if (value < 100)
			prim2->body->contacts[value++] = contact;
	}




	contact->maxPenetration = contact->points[0]->penetration;
	int i = contact->pcount - 1;
	while (i >= 1)
	{
		double pen = contact->points[i]->penetration;
		if (pen > contact->maxPenetration)
			contact->maxPenetration = pen;

		i--;
	}

	return true;
}

bool TanksCollisionDetector::testCollision(CollisionPrimitive *prim1, CollisionPrimitive *prim2)
{
	_function_name("TanksCollisionDetector::testCollision");

	if ((prim1->collisionGroup & prim2->collisionGroup) == 0)
	{
		return false;
	}
	if (prim1->body != nullptr && prim1->body == prim2->body)
	{
		return false;
	}
	if (!prim1->aabb.intersects(&prim2->aabb, 0.01))
	{
		return false;
	}

	ICollider *collider = getCollider(this->colliders, prim1, prim2);
	if (collider != nullptr && collider->haveCollision(prim1, prim2))
	{
		if (prim1->postCollisionPredicate != nullptr && !prim1->postCollisionPredicate->considerCollision(prim2))
		{
			return false;
		}
		if (prim2->postCollisionPredicate != nullptr && !prim2->postCollisionPredicate->considerCollision(prim1))
		{
			return false;
		}
		return true;
	}
	return false;
}

void TanksCollisionDetector::destroy()
{
	_function_name("TanksCollisionDetector::destroy");
	delete this;
}


void TanksCollisionDetector::addBody(Body *body)
{
	_function_name("TanksCollisionDetector::addBody");

	if (this->bodies->findItem(body) != nullptr)
		return;
	this->bodies->append(body);
}

void TanksCollisionDetector::removeBody(Body *body)
{
	_function_name("TanksCollisionDetector::removeBody");
	this->bodies->remove(body);
}

Contact* TanksCollisionDetector::getPrimitiveNodeCollisions(CollisionKdNode *node, CollisionPrimitive *primitive, Contact *contact)
{
	_function_name("TanksCollisionDetector::getPrimitiveNodeCollisions");

	if (node->indices != nullptr)
	{
		QVector<CollisionPrimitive*> *primitives = this->tree->staticChildren;
		QVector<int> *indices = node->indices;
		for (int i = indices->length() - 1; i >= 0; i--)
		{
			CollisionPrimitive *p = primitives->at(indices->at(i));
			if (this->getContact(primitive, p, contact))
			{
				contact = contact->next;
			}
		}
	}
	if (node->axis == -1)
	{
		return contact;
	}

	double min, max;

	switch (node->axis)
	{
		case 0:
			min = primitive->aabb.minX;
			max = primitive->aabb.maxX;
			break;
		case 1:
			min = primitive->aabb.minY;
			max = primitive->aabb.maxY;
			break;
		case 2:
			min = primitive->aabb.minZ;
			max = primitive->aabb.maxZ;
			break;
		default:
			throw 10514;
	}

	if (min < node->coord)
	{
		contact = this->getPrimitiveNodeCollisions(node->negativeNode, primitive, contact);
	}
	if (max > node->coord)
	{
		contact = this->getPrimitiveNodeCollisions(node->positiveNode, primitive, contact);
	}
	if (node->splitTree != nullptr && min < node->coord && max > node->coord)
	{
		contact = this->getPrimitiveNodeCollisions(node->splitTree->rootNode, primitive, contact);
	}
	return contact;
}

bool TanksCollisionDetector::intersectRayWithDynamic(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, IRayCollisionPredicate *predicate, RayIntersection *result)
{
	_function_name("TanksCollisionDetector::intersectRayWithDynamic");

	const double xx = origin->x + (dir->x * maxTime);
	const double yy = origin->y + (dir->y * maxTime);
	const double zz = origin->z + (dir->z * maxTime);

	BoundBox rayAABB;

	if (xx < origin->x)
	{
		rayAABB.minX = xx;
		rayAABB.maxX = origin->x;
	}
	else
	{
		rayAABB.minX = origin->x;
		rayAABB.maxX = xx;
	}

	if (yy < origin->y)
	{
		rayAABB.minY = yy;
		rayAABB.maxY = origin->y;
	}
	else
	{
		rayAABB.minY = origin->y;
		rayAABB.maxY = yy;
	}

	if (zz < origin->z)
	{
		rayAABB.minZ = zz;
		rayAABB.maxZ = origin->z;
	}
	else
	{
		rayAABB.minZ = origin->z;
		rayAABB.maxZ = zz;
	}

	double minTime = maxTime + 1.;
	BodyListItem *bodyItem = this->bodies->head;
	while (bodyItem != nullptr)
	{
		Body *body = bodyItem->body;
		BoundBox *aabb = &body->aabb;
		if (rayAABB.maxX < aabb->minX || rayAABB.minX > aabb->maxX || rayAABB.maxY < aabb->minY || rayAABB.minY > aabb->maxY || rayAABB.maxZ < aabb->minZ || rayAABB.minZ > aabb->maxZ)
		{
			bodyItem = bodyItem->next;
		}
		else
		{
			CollisionPrimitiveListItem *listItem = body->collisionPrimitives->head;
			while (listItem != nullptr)
			{
				CollisionPrimitive *primitive = listItem->primitive;
				if ((primitive->collisionGroup & collisionGroup) == 0)
				{
					listItem = listItem->next;
				}
				else
				{
					aabb = &primitive->aabb;
					if (rayAABB.maxX < aabb->minX || rayAABB.minX > aabb->maxX || rayAABB.maxY < aabb->minY || rayAABB.minY > aabb->maxY || rayAABB.maxZ < aabb->minZ || rayAABB.minZ > aabb->maxZ)
					{
						listItem = listItem->next;
					}
					else
					{
						if (predicate != nullptr && !predicate->considerBody(body))
						{
							listItem = listItem->next;
						}
						else
						{
							Vector3 _n;
							_n.reset();
							double t = primitive->getRayIntersection(origin, dir, this->threshold, &_n);
							if (t >= 0. && t < minTime)
							{
								minTime = t;
								result->primitive = primitive;
								result->normal.x = _n.x;
								result->normal.y = _n.y;
								result->normal.z = _n.z;
							}
							listItem = listItem->next;
						}
					}
				}
			}
			bodyItem = bodyItem->next;
		}
	}
	if (minTime > maxTime)
	{
		return false;
	}
	result->pos.x = origin->x + (dir->x * minTime);
	result->pos.y = origin->y + (dir->y * minTime);
	result->pos.z = origin->z + (dir->z * minTime);
	result->t = minTime;

	return true;
}

bool TanksCollisionDetector::getRayBoundBoxIntersection(const Vector3 *origin, const Vector3 *dir, const BoundBox *bb, MinMax *time)
{
	_function_name("TanksCollisionDetector::getRayBoundBoxIntersection");

	double t1;
	double t2;
	time->min = -1;
	time->max = 1E308;

	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
			case 0:
				if (dir->x < this->threshold && dir->x > -this->threshold)
				{
					if (origin->x < bb->minX || origin->x > bb->maxX)
					{
						return false;
					}
					continue;
				}
				t1 = (bb->minX - origin->x) / dir->x;
				t2 = (bb->maxX - origin->x) / dir->x;
				break;
			case 1:
				if (dir->y < this->threshold && dir->y > -this->threshold)
				{
					if (origin->y < bb->minY || origin->y > bb->maxY)
					{
						return false;
					}
					continue;
				}
				t1 = (bb->minY - origin->y) / dir->y;
				t2 = (bb->maxY - origin->y) / dir->y;
				break;
			case 2:
				if (dir->z < this->threshold && dir->z > -this->threshold)
				{
					if (origin->z < bb->minZ || origin->z > bb->maxZ)
					{
						return false;
					}
					continue;
				}
				t1 = (bb->minZ - origin->z) / dir->z;
				t2 = (bb->maxZ - origin->z) / dir->z;
				break;
			default:
				throw 52325;
		}
		if (t1 < t2)
		{
			if (t1 > time->min)
			{
				time->min = t1;
			}
			if (t2 < time->max)
			{
				time->max = t2;
			}
		}
		else
		{
			if (t2 > time->min)
			{
				time->min = t2;
			}
			if (t1 < time->max)
			{
				time->max = t1;
			}
		}
		if (time->max < time->min)
		{
			return false;
		}
	}

	return true;
}

bool TanksCollisionDetector::testRayAgainstNode(CollisionKdNode *node, const Vector3 *origin, const Vector3 *localOrigin, const Vector3 *dir, int collisionGroup, double t1, double t2, IRayCollisionPredicate *predicate, RayIntersection *result)
{
	_function_name("TanksCollisionDetector::testRayAgainstNode");

	double splitTime;
	CollisionKdNode *currChildNode;

	if (node->indices != nullptr && this->getRayNodeIntersection(origin, dir, collisionGroup, this->tree->staticChildren, node->indices, predicate, result))
	{
		return true;
	}

	if (node->axis == -1)
	{
		return false;
	}

	switch (node->axis)
	{
		case 0:
			if (dir->x > -this->threshold && dir->x < this->threshold)
			{
				splitTime = t2 + 1;
			}
			else
			{
				splitTime = (node->coord - origin->x) / dir->x;
			}
			currChildNode = (localOrigin->x < node->coord) ? node->negativeNode : node->positiveNode;
			break;
		case 1:
			if (dir->y > -(this->threshold) && (dir->y < this->threshold))
			{
				splitTime = t2 + 1;
			}
			else
			{
				splitTime = (node->coord - origin->y) / dir->y;
			}
			currChildNode = (localOrigin->y < node->coord) ? node->negativeNode : node->positiveNode;
			break;
		case 2:
			if (dir->z > -this->threshold && dir->z < this->threshold)
			{
				splitTime = t2 + 1.;
			}
			else
			{
				splitTime = (node->coord - origin->z) / dir->z;
			}
			currChildNode = (localOrigin->z < node->coord) ? node->negativeNode : node->positiveNode;
			break;
		default:
			throw 5135;
	}

	volatile double _splitTime = splitTime;

	if (_splitTime < t1 || _splitTime > t2)
	//if (splitTime < t1 || splitTime > t2) //problem
	{
		return this->testRayAgainstNode(currChildNode, origin, localOrigin, dir, collisionGroup, t1, t2, predicate, result);
	}

	bool intersects = this->testRayAgainstNode(currChildNode, origin, localOrigin, dir, collisionGroup, t1, splitTime, predicate, result);
	if (intersects)
	{
		return true;
	}

	Vector3 _o;
	_o.x = origin->x + (splitTime * dir->x);
	_o.y = origin->y + (splitTime * dir->y);
	_o.z = origin->z + (splitTime * dir->z);

	if (node->splitTree != nullptr)
	{
		CollisionKdNode *splitNode = node->splitTree->rootNode;
		while (splitNode != nullptr && splitNode->axis != -1)
		{
			switch (splitNode->axis)
			{
				case 0:
					splitNode = (_o.x < splitNode->coord) ? splitNode->negativeNode : splitNode->positiveNode;
					break;
				case 1:
					splitNode = (_o.y < splitNode->coord) ? splitNode->negativeNode : splitNode->positiveNode;
					break;
				case 2:
					splitNode = (_o.z < splitNode->coord) ? splitNode->negativeNode : splitNode->positiveNode;
					break;
				default:
					throw 52453;
			}
		}

		if (splitNode != nullptr && splitNode->indices != nullptr)
		{
			for (int i = splitNode->indices->length() - 1; i >= 0; i--)
			{
				CollisionPrimitive *primitive = this->tree->staticChildren->at(splitNode->indices->at(i));
				if ((primitive->collisionGroup & collisionGroup) != 0)
				{
					if (!(predicate != nullptr && primitive->body != nullptr && !predicate->considerBody(primitive->body)))
					{
						result->t = primitive->getRayIntersection(origin, dir, this->threshold, &result->normal);
						if (result->t >= 0.)
						{
							result->pos.copy(&_o);
							result->primitive = primitive;
							return true;
						}
					}
				}
			}
		}
	}

	return this->testRayAgainstNode((currChildNode == node->negativeNode) ? node->positiveNode : node->negativeNode, origin, &_o, dir, collisionGroup, splitTime, t2, predicate, result);
}


bool TanksCollisionDetector::getRayNodeIntersection(const Vector3 *origin, const Vector3 *dir, int collisionGroup, QVector<CollisionPrimitive*> *primitives, QVector<int> *indices, IRayCollisionPredicate *predicate, RayIntersection *intersection)
{
	_function_name("TanksCollisionDetector::getRayNodeIntersection");

	int pnum = indices->length();
	double minTime = 1E308;

	for (int i = 0; i < pnum; i++)
	{
		CollisionPrimitive *primitive = primitives->at(indices->at(i));
		if ((primitive->collisionGroup & collisionGroup) != 0)
		{
			if (!(predicate != nullptr && primitive->body != nullptr && !predicate->considerBody(primitive->body)))
			{
				Vector3 n;
				double t = primitive->getRayIntersection(origin, dir, this->threshold, &n);
				if (t > 0. && t < minTime)
				{
					minTime = t;
					intersection->primitive = primitive;
					intersection->normal.copy(&n);
				}
			}
		}
	}

	if (minTime == 1E308)
		return false;

	intersection->pos.x = origin->x + (dir->x * minTime);
	intersection->pos.y = origin->y + (dir->y * minTime);
	intersection->pos.z = origin->z + (dir->z * minTime);
	intersection->t = minTime;
	return true;
}


void TanksCollisionDetector::buildKdTree(std::shared_ptr<QVector<std::shared_ptr<CollisionPrimitive>>> collisionPrimitives, BoundBox *boundBox)
{
	_function_name("TanksCollisionDetector::buildKdTree");

	this->tree->createTree(collisionPrimitives, boundBox);
}

bool TanksCollisionDetector::raycastStatic(const Vector3 *origin, const Vector3 *dir, int collisionGroup, double maxTime, RayIntersection *result)
{
	MinMax time;

	if ((!(this->getRayBoundBoxIntersection(origin, dir, &this->tree->rootNode->boundBox, &time))))
	{
		return false;
	}

	if (((time.max < 0) || (time.min > maxTime)))
	{
		return false;
	}

	Vector3 _o;

	if (time.min <= 0)
	{
		time.min = 0;
		_o.x = origin->x;
		_o.y = origin->y;
		_o.z = origin->z;
	}
	else
	{
		_o.x = origin->x + (time.min * dir->x);
		_o.y = origin->y + (time.min * dir->y);
		_o.z = origin->z + (time.min * dir->z);
	}

	if (time.max > maxTime)
	{
		time.max = maxTime;
	}

	bool _loc7_ = this->testRayAgainstNode(this->tree->rootNode, origin, &_o, dir, collisionGroup, time.min, time.max, nullptr, result);
	return ((!(!(_loc7_))) ? (result->t <= maxTime) : false);
}

bool TanksCollisionDetector::hasStaticHit(Vector3 *origin, Vector3 *dir, int collisionGroup, double maxTime)
{
	return this->raycastStatic(origin, dir, collisionGroup, maxTime, this->_rayHit);
}
