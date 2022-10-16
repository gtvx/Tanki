#ifndef DAYGAMEMODE_H
#define DAYGAMEMODE_H

#include "IGameMode.h"
#include <memory>

class GameCamera;
class DirectionalLight;

class DayGameMode : public IGameMode
{
	GameCamera *camera;
	std::shared_ptr<DirectionalLight> light;
public:
	DayGameMode();

	void applyChanges(BattleView3D *viewport) override;
	void applyChangesBeforeSettings(IBattleSettings *settings) override;
	BitmapData* applyColorchangesToSkybox(BitmapData *skybox) override;
};

#endif // DAYGAMEMODE_H
