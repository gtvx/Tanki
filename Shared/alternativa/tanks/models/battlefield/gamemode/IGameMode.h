#ifndef IGAMEMODE_H
#define IGAMEMODE_H

class BattleView3D;
class IBattleSettings;
class BitmapData;


class IGameMode
{
public:
	virtual void applyChanges(BattleView3D*) = 0;
	virtual void applyChangesBeforeSettings(IBattleSettings*) = 0;
	virtual BitmapData* applyColorchangesToSkybox(BitmapData*) = 0;
};

#endif // IGAMEMODE_H
