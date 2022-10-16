#include "defaultgamemodel.h"
#include "../battleview3d.h"
#include "alternativa/tanks/camera/gamecamera.h"


void DefaultGameModel::applyChanges(BattleView3D *viewport)
{
	viewport->camera->useLight = false;
	viewport->camera->useShadowMap = false;
	viewport->camera->deferredLighting = false;
}

void DefaultGameModel::applyChangesBeforeSettings(IBattleSettings *settings)
{
	(void)settings;
}

BitmapData* DefaultGameModel::applyColorchangesToSkybox(BitmapData *skybox)
{
	return skybox;
}
