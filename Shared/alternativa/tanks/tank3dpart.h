#ifndef TANK3DPART_H
#define TANK3DPART_H

#include <memory>

class Vector3;
class Mesh;
class BitmapData;

class Tank3DPart
{
public:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<BitmapData> lightmap;
	std::shared_ptr<BitmapData> details;
	std::shared_ptr<Vector3> turretMountPoint; //Vector3D
	bool animatedPaint;

	Tank3DPart();
};

#endif // TANK3DPART_H
