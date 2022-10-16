#include "lightbeam.h"
#include "alternativa/engine3d/core/Sorting.h"
#include "alternativa/engine3d/materials/texturematerial.h"

const Object3D::T *LightBeam::TYPE = new Object3D::T("LightBeam");


LightBeam::LightBeam(const TT &t, double size) :
	Mesh(TT(t.type, this->TYPE)),
	__type_next(t.type_next)
{
	double v = size / 2;
	std::shared_ptr<Vertex> v1 = addVertex(-v, 0, (size + 0), 0, 0);
	std::shared_ptr<Vertex> v2 = addVertex(-v, 0, (0 + 0), 0, 1);
	std::shared_ptr<Vertex> v3 = addVertex(v, 0, (0 + 0), 1, 1);
	std::shared_ptr<Vertex> v4 = addVertex(v, 0, (size + 0), 1, 0);
	addQuadFace(v1, v2, v3, v4);
	sorting = Sorting::DYNAMIC_BSP;
	calculateBounds();
	calculateFacesNormals();
}

void LightBeam::init(TextureMaterial *material)
{
	setMaterialToAllFaces(material);
}

void LightBeam::clear()
{
	setMaterialToAllFaces(nullptr);
}

std::shared_ptr<LightBeam> new_LightBeam(double size)
{
	return std::make_shared<LightBeam>(Object3D::TT(LightBeam::TYPE), size);
}
