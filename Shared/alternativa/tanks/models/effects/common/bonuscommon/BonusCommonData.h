#ifndef BONUSCOMMONDATA_H
#define BONUSCOMMONDATA_H

#include <memory>

class Mesh;
class Material;

class BonusCommonData
{
public:
	std::shared_ptr<Mesh> boxMesh;
	std::shared_ptr<Mesh> parachuteMesh;
	//std::shared_ptr<Mesh> parachuteInnerMesh;
	Material *cordMaterial;
	int duration;
};

#endif // BONUSCOMMONDATA_H
