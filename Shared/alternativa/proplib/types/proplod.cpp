#include "proplod.h"
#include "../objects/propobject.h"

void PropLOD::traceLOD()
{
	//trace("Lod distance", this.distance);
	this->prop->traceProp();
}
