#ifndef DEFAULTGAMEMODEL_H
#define DEFAULTGAMEMODEL_H

#include "IGameMode.h"

class DefaultGameModel : public IGameMode
{
public:
	void applyChanges(BattleView3D *viewport) override;
	void applyChangesBeforeSettings(IBattleSettings *settings) override;
	BitmapData* applyColorchangesToSkybox(BitmapData *skybox) override;
};

#endif // DEFAULTGAMEMODEL_H
