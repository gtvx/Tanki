#include "controlpointmarkers.h"
#include "controlpointsprite.h"
#include "../dompoint.h"
#include "controlpointsprite.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "__global.h"


ControlPointMarkers::ControlPointMarkers(IBattleField *battlefieldModel, const Vector3 *pos, DomPoint *keypoint)
{
#ifdef GRAPHICS
	ControlPointSprite::init();

	int pointId = keypoint->id.value;

#ifdef L_GT
	pointId -= 'A';
#endif

	this->point = keypoint;
	this->sprite = new_ControlPointSprite(pointId);

	this->sprite->redraw();
	this->sprite->position.copy(pos);
	battlefieldModel->getBattlefieldData()->viewport->getMapContainer()->addChild(this->sprite.get());
#else
	(void)battlefieldModel;
	(void)pos;
	(void)keypoint;
#endif
}

void ControlPointMarkers::tick()
{
#ifdef GRAPHICS
	this->sprite->progress = this->point->clientProgress;
	this->sprite->redraw();
#endif
}

void ControlPointMarkers::destroy()
{
#ifdef GRAPHICS
	this->sprite->destroy();
	this->sprite = nullptr;
#endif
}
