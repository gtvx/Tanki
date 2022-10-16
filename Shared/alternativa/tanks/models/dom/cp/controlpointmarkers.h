#ifndef CONTROLPOINTMARKERS_H
#define CONTROLPOINTMARKERS_H

#include <memory>

class DomPoint;
class ControlPointSprite;
class IBattleField;
class Vector3;

class ControlPointMarkers
{
	DomPoint *point;
public:
	std::shared_ptr<ControlPointSprite> sprite;
	ControlPointMarkers(IBattleField *battlefieldModel, const Vector3 *pos, DomPoint *keypoint);
	void tick();
	void destroy();
};

#endif // CONTROLPOINTMARKERS_H
