#include "parachute2.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/core/object3d_clone.h"
#include "bonuscache.h"

const Object3D::T *Parachute2::TYPE = new Object3D::T("Parachute2");


const double Parachute2::RADIUS = 266;
const int Parachute2::NUM_STRAPS = 12;



Parachute2::Parachute2(const TT &t, Mesh *outerMeshSource, Mesh *innerMeshSource) :
	Object3DContainer(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	ownAlpha = 1;
	alphaMultiplier = 1;

	(void)innerMeshSource;

	mesh1 = Mesh_clone(outerMeshSource);

	this->addMesh(mesh1.get());
	//this->addMesh(Mesh(innerMeshSource.clone()));
}

void Parachute2::addMesh(Mesh *mesh)
{
	mesh->shadowMapAlphaThreshold = 0.1;
	mesh->calculateVerticesNormalsBySmoothingGroups(0.01);
	addChild(mesh);
	mesh->setName("parachute");
}

void Parachute2::recycle()
{
	this->ownAlpha = 1;
	this->alphaMultiplier = 1;
	scale.reset(1, 1, 1);
	alpha = 1;
	BonusCache::putParachute(std::dynamic_pointer_cast<Parachute2>(this->ptr()));
}

void Parachute2::setAlpha(double value)
{
	this->ownAlpha = value;
	this->updateAlpha();
}

void Parachute2::updateAlpha()
{
	alpha = (this->alphaMultiplier * this->ownAlpha);
}

void Parachute2::readPosition(Vector3 *result)
{
	result->copy(&this->position);
}

void Parachute2::setAlphaMultiplier(double value)
{
	this->alphaMultiplier = value;
	this->updateAlpha();
}



std::shared_ptr<Parachute2> new_Parachute2(Mesh *outerMeshSource, Mesh *innerMeshSource)
{
	return std::make_shared<Parachute2>(Object3D::TT(Parachute2::TYPE), outerMeshSource, innerMeshSource);
}
