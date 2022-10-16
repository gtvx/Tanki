#include "landingstate.h"

LandingState::LandingState()
{
	this->position.reset();
	this->orientation.reset();
}

void LandingState::interpolate(LandingState *a, LandingState *b, double t)
{
	this->position.interpolate(t, &a->position, &b->position);
	this->orientation.slerp(&a->orientation, &b->orientation, t);
}

void LandingState::copy(LandingState *src)
{
	this->position.copy(&src->position);
	this->orientation.copy(&src->orientation);
}
