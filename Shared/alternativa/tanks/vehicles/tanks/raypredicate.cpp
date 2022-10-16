#include "raypredicate.h"


RayPredicate::RayPredicate(Body *body)
{
	this->body = body;
}

RayPredicate::~RayPredicate()
{

}

bool RayPredicate::considerBody(Body *body)
{
	return this->body != body;
}
