#include "tankskinpart.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/engine3d/core/Sorting.h"

void TankSkinPart::initMesh()
{
	if (this->mesh->sorting == Sorting::DYNAMIC_BSP)
		return;

	this->mesh->sorting = Sorting::DYNAMIC_BSP;
	this->mesh->calculateFacesNormals(true);
	//this->mesh->optimizeForDynamicBSP();
	//this->mesh->threshold = 0.01;
	//this->resolution = this->mesh->calculateResolution(this->details->width, this->details->height);
}

TankSkinPart::TankSkinPart(std::shared_ptr<Mesh> &mesh, bool isHull)
{
	(void)isHull;
	//this.details = new BitmapData(100, 100);
	//this.lightmap = new BitmapData(100, 100);
	//this.detailsHull = new BitmapData(100, 100);
	//this.lightmapHull = new BitmapData(100, 100);
	this->mesh = mesh;
	this->initMesh();
}

TankSkinPart::~TankSkinPart()
{

}

Mesh *TankSkinPart::getMesh()
{
	throw 13534;
}
