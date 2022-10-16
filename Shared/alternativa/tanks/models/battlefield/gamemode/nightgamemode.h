#ifndef NIGHTGAMEMODE_H
#define NIGHTGAMEMODE_H

#include "IGameMode.h"
#include <memory>

class GameCamera;
class DirectionalLight;

class NightGameMode : public IGameMode
{
	std::shared_ptr<DirectionalLight> light;
	GameCamera *camera;

public:

	void applyChanges(BattleView3D *viewport) override;
	void applyChangesBeforeSettings(IBattleSettings *settings) override;
	BitmapData* applyColorchangesToSkybox(BitmapData *skybox) override;
};

#endif // NIGHTGAMEMODE_H
