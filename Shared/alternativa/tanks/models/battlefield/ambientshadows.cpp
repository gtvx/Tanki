#include "ambientshadows.h"
#include "alternativa/engine3d/core/camera3d.h"


AmbientShadows::AmbientShadows(Camera3D *camera)
{
	this->camera = camera;
	this->enabled = false;
}

void AmbientShadows::add(Shadow *shadow)
{
	this->shadows.insert(shadow);

	if (this->enabled)
		this->camera->addShadow(shadow);
}

void AmbientShadows::remove(Shadow *shadow)
{
	this->shadows.remove(shadow);

	if (this->enabled)
		this->camera->removeShadow(shadow);
}

void AmbientShadows::enable()
{
	if (this->enabled == true)
		return;

	this->enabled = true;

	for (Shadow *shadow : this->shadows)
		this->camera->addShadow(shadow);
}

void AmbientShadows::disable()
{
	if (this->enabled == false)
		return;

	this->enabled = false;

	for (Shadow *shadow : this->shadows)
		this->camera->removeShadow(shadow);
}
