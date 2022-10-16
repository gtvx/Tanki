#ifndef TANKSKINPART_H
#define TANKSKINPART_H

#include <memory>

class Mesh;

class TankSkinPart
{
	void initMesh();
public:


	//Long *partId;
	//BitmapData details;
	//BitmapData lightmap;
	//BitmapData detailsHull;
	//BitmapDatar lightmapHull;
	std::shared_ptr<Mesh> mesh;
	double resolution;

	TankSkinPart(std::shared_ptr<Mesh> &mesh, bool isHull = false);
	virtual ~TankSkinPart();

protected:
	virtual Mesh* getMesh();
};

#endif // TANKSKINPART_H
