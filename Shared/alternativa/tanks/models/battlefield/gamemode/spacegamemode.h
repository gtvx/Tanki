#ifndef SPACEGAMEMODE_H
#define SPACEGAMEMODE_H

#include "IGameMode.h"
#include <memory>

class GameCamera;
class DirectionalLight;

class SpaceGameMode : public IGameMode
{
	std::shared_ptr<DirectionalLight> light;
	GameCamera *camera;
public:
	void applyChanges(BattleView3D *viewport) override;
	void applyChangesBeforeSettings(IBattleSettings *settings) override;
	BitmapData* applyColorchangesToSkybox(BitmapData *skybox) override;
};

#endif // SPACEGAMEMODE_H
