#ifndef PANELMODEL_H
#define PANELMODEL_H

#include "forms/events/MainButtonBarEvents.h"
#include "IBattleSettings.h"

class ClientObject;
class QString;
class Network;
class MainPanel;
class Stage;


class PanelModel : public  IBattleSettings
{
	QString *_userName;
	ClientObject *clientObject;
	Network *networker;
	MainPanel *mainPanel;
	bool isInBattle;
public:
	PanelModel();
	~PanelModel();

	bool isGarageSelect;
	bool isBattleSelect;

	void initObject(Network *network, ClientObject *clientObject, int crystal, const QString &email, bool isTester, const QString &name, int nextScore, int place, int rank, double rating, int score);
	void updateRank(ClientObject *clientObject, int rank, int nextScore);
	void updateCrystal(ClientObject *clientObject, int crystal);

	QString getUserName();
	void objectLoaded(ClientObject *object);
	void onExitFromBattle(bool sendToServer = true);
	void lock();
	void unlock();
	void showGarage();
	void onButtonBarButtonClick(MainButtonBarEvents e);

	void clickExit();
	void clickBattles();
	void clickGarage();
	void showBattleSelect(ClientObject *c, bool sendToServer = true);
	void setCurrentGarage();
	void setCurrentBattles();
	void setCurrentBattle();
	void draw(Stage *stage);


	bool showSkyBox() override;
	bool showFPS() override;
	bool showBattleChat() override;
	bool adaptiveFPS() override;
	bool enableMipMapping() override;
	bool inverseBackDriving() override;
	bool bgSound() override;
	bool muteSound() override;
	bool showShadowsTank() override;
	bool fog() override;
	bool dust() override;
	bool useSoftParticle() override;
	bool shadows() override;
	bool defferedLighting() override;
	bool animationTracks() override;
	bool shadowUnderTanks() override;
	bool coloredFPS() override;
	bool useSSAO() override;
};

#endif // PANELMODEL_H
