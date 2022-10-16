#include "aim.h"

Aim::Aim(Tank *b, const Vector3 *v)
{
	this->target = b;
	this->targetHitPoint.copy(v);
}
