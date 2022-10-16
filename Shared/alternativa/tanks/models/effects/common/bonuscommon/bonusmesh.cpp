#include "bonusmesh.h"
#include "alternativa/engine3d/core/Sorting.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/core/face.h"
#include "bonuscache.h"
#include "_global.h"
#include <QString>


const Object3D::T *BonusMesh::TYPE = new Object3D::T("BonusMesh");


BonusMesh::BonusMesh(const TT &t, const QString &objectId, Mesh *sourceMesh) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	_function_name("BonusMesh::BonusMesh");
	ownAlpha = 1.;
	alphaMultiplier = 1.;

	this->objectId = new QString(objectId);
	clonePropertiesFromMesh(sourceMesh);

	//Face *face = sourceMesh->faces[0];
	Face *face = sourceMesh->faceList().get();
	setMaterialToAllFaces(face->material.get());
	shadowMapAlphaThreshold = 0.1;

	sorting = Sorting::DYNAMIC_BSP;
	this->setName("bonus");
	rotation.reset();
	scale.reset(1, 1, 1);
	this->ownAlpha = 1;
	this->alphaMultiplier = 1;
	this->updateAlpha();
}

BonusMesh::~BonusMesh()
{
	_function_name("BonusMesh::~BonusMesh");
	delete objectId;
}

QString BonusMesh::getObjectId()
{
	_function_name("BonusMesh::getObjectId");
	return *this->objectId;
}

double BonusMesh::getAlpha()
{
	_function_name("BonusMesh::getAlpha");
	return this->ownAlpha;
}

void BonusMesh::setAlpha(double value)
{
	_function_name("BonusMesh::setAlpha");
	this->ownAlpha = value;
	this->updateAlpha();
}

void BonusMesh::readPosition(Vector3 *result)
{
	_function_name("BonusMesh::readPosition");
	result->copy(&position);
}

void BonusMesh::setAlphaMultiplier(double value)
{
	_function_name("BonusMesh::setAlphaMultiplier");
	this->alphaMultiplier = value;
	this->updateAlpha();
}

void BonusMesh::updateAlpha()
{
	_function_name("BonusMesh::updateAlpha");
	alpha = this->alphaMultiplier * this->ownAlpha;
}

std::shared_ptr<BonusMesh> new_BonusMesh(const QString &objectId, Mesh *sourceMesh)
{
	_function_name("new_BonusMesh");
	return std::make_shared<BonusMesh>(Object3D::TT(BonusMesh::TYPE), objectId, sourceMesh);
}

void BonusMesh::recycle()
{
	_function_name("BonusMesh::recycle");
	this->ownAlpha = 1;
	this->alphaMultiplier = 1;
	BonusCache::putBonusMesh(std::dynamic_pointer_cast<BonusMesh>(this->ptr()));
}
