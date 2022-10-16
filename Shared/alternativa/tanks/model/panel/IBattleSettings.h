#ifndef IBATTLESETTINGS_H
#define IBATTLESETTINGS_H

class IBattleSettings
{
public:
	virtual bool showSkyBox() = 0;
	virtual bool showFPS() = 0;
	virtual bool showBattleChat() = 0;
	virtual bool adaptiveFPS() = 0;
	virtual bool enableMipMapping() = 0;
	virtual bool inverseBackDriving() = 0;
	virtual bool bgSound() = 0;
	virtual bool muteSound() = 0;
	virtual bool showShadowsTank() = 0;
	virtual bool fog() = 0;
	virtual bool dust() = 0;
	virtual bool useSoftParticle() = 0;
	virtual bool shadows() = 0;
	virtual bool defferedLighting() = 0;
	virtual bool animationTracks() = 0;
	virtual bool shadowUnderTanks() = 0;
	virtual bool coloredFPS() = 0;
	virtual bool useSSAO() = 0;
};

#endif // IBATTLESETTINGS_H
