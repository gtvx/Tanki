#ifndef RAYPREDICATE_H
#define RAYPREDICATE_H

#include "alternativa/physics/collision/IRayCollisionPredicate.h"

class RayPredicate : public IRayCollisionPredicate
{
	Body *body;
public:
	RayPredicate(Body *body);
	virtual ~RayPredicate();
	bool considerBody(Body *body);
};

#endif // RAYPREDICATE_H
