#include "panelmodel.h"
#include "service.h"
#include "IMain.h"
#include "networking/network.h"
#include "forms/events/MainButtonBarEvents.h"
#include "lobby.h"
#include "alternativa/tanks/model/battleselectmodel.h"
#include <QString>
#include <QDebug>
#include "alternativa/engine3d/containers/kdcontainer.h"
#include "alternativa/tanks/models/ctf/ctfmodel.h"
#include "alternativa/tanks/models/dom/dommodel.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "battlecontroller.h"
#include "forms/mainpanel.h"
#include "__global.h"

namespace
{
	//Тени под танком
	bool STATUS_showShadowsTank = false;

	//Пыль
	bool STATUS_dust = false;

	//Туман
	bool STATUS_fog = false;

	//Мягкие частицы
	bool STATUS_useSoftParticle = false;

	//Тени
	bool STATUS_shadows = false;

	//Освещение
	bool STATUS_defferedLighting = false;

	//Анимированные гусеницы
	bool STATUS_animationTracks = true;

	//Тени под танком
	bool STATUS_shadowUnderTanks = false;

	//SSAO
	bool STATUS_useSSAO = false;
}



PanelModel::PanelModel()
{
	isInBattle = false;
#ifdef GRAPHICS
	this->mainPanel = new MainPanel();
#endif
}

PanelModel::~PanelModel()
{
#ifdef GRAPHICS
	delete this->mainPanel;
#endif
}

void PanelModel::draw(Stage *stage)
{
	(void)stage;
#ifdef GRAPHICS
	this->mainPanel->draw(stage);
#endif
}

bool PanelModel::showSkyBox()
{
	return true;
}

bool PanelModel::showFPS()
{
	return true;
}

bool PanelModel::showBattleChat()
{
	return true;
}

bool PanelModel::adaptiveFPS()
{
	return true;
}

bool PanelModel::enableMipMapping()
{
	return true;
}

bool PanelModel::inverseBackDriving()
{
	return false;
}

bool PanelModel::bgSound()
{
	return true;
}

bool PanelModel::muteSound()
{
	return false;
}


bool PanelModel::showShadowsTank()
{
	return STATUS_showShadowsTank;
}

bool PanelModel::fog()
{
	return STATUS_fog;
}

bool PanelModel::dust()
{
	return STATUS_dust;
}

bool PanelModel::useSoftParticle()
{
	return STATUS_useSoftParticle;
}

bool PanelModel::shadows()
{
	return STATUS_shadows;
}

bool PanelModel::defferedLighting()
{
	return STATUS_defferedLighting;
}

bool PanelModel::animationTracks()
{
	return STATUS_animationTracks;
}

bool PanelModel::shadowUnderTanks()
{
	return STATUS_shadowUnderTanks;
}

bool PanelModel::coloredFPS()
{
	return true;
}

bool PanelModel::useSSAO()
{
	return STATUS_useSSAO;
}


void PanelModel::initObject(Network *network, ClientObject *clientObject, int crystal, const QString &email, bool isTester, const QString &name, int nextScore, int place, int rank, double rating, int score)
{
	(void)crystal;
	(void)email;
	(void)isTester;
	(void)nextScore;
	(void)place;
	(void)rank;
	(void)rating;
	(void)score;

	this->networker = network;

	this->isGarageSelect = false;
	this->isBattleSelect = false;

	IMain *imain = Service::getIMain();
	imain->setNick(name);
	imain->setRank(rank);
	imain->setCrystal(crystal);
#ifdef GRAPHICS
	this->mainPanel->setRang(rank);
#endif

	_userName = new QString(name);
	objectLoaded(clientObject);
}

void PanelModel::updateRank(ClientObject *clientObject, int rank, int nextScore)
{
	(void)clientObject;
	(void)nextScore;
	IMain *imain = Service::getIMain();
	imain->setRank(rank);
}

void PanelModel::updateCrystal(ClientObject *clientObject, int crystal)
{
	(void)clientObject;
	IMain *imain = Service::getIMain();
	imain->setCrystal(crystal);
}

QString PanelModel::getUserName()
{
	return *_userName;
}

void PanelModel::objectLoaded(ClientObject *object)
{
	//this->networker = Network(Main.osgi.getService(INetworker));
	this->clientObject = object;
	//this->moneyListeners = new Array();
	//Main.osgi.registerService(IMoneyService, this);
	//this->setSoundSettings();
	//this->mainPanel.buttonBar.soundOn = (!(this->getSoundMute()));
	//IHelpService helpService = (Main.osgi.getService(IHelpService) as IHelpService);
	//this->rankHelper = new RankHelper();
	//this->rankBarHelper = new RankBarHelper();
	//this->ratingHelper = new RatingIndicatorHelper();
	//this->mainMenuHelper = new MainMenuHelper();
	//this->buttonBarHelper = new ButtonBarHelper();
	//this->moneyHelper = new MoneyHelper();
	//this->scoreHelper = new ScoreHelper();
	//this->unlock();
	//helpService.registerHelper(HELPER_GROUP_KEY, this->HELPER_RANK, this->rankHelper, true);
	//helpService.registerHelper(HELPER_GROUP_KEY, this->HELPER_RANK_BAR, this->rankBarHelper, true);
	//helpService.registerHelper(HELPER_GROUP_KEY, this->HELPER_RATING_INDICATOR, this->ratingHelper, true);
	//helpService.registerHelper(HELPER_GROUP_KEY, this->HELPER_MAIN_MENU, this->mainMenuHelper, true);
	//helpService.registerHelper(HELPER_GROUP_KEY, this->HELPER_BUTTON_BAR, this->buttonBarHelper, true);
	//helpService.registerHelper(HELPER_GROUP_KEY, this->HELPER_SCORE, this->scoreHelper, true);
}



void PanelModel::onExitFromBattle(bool sendToServer)
{
	/*
	BattlefieldModel *bfModel = Service::getBattlefieldModel();
	if (bfModel == nullptr)
		return;




	bfModel->objectUnloaded(nullptr);
	*/

	/*
	if (bfModel->toDestroy->length > 1)
	{
		for each (o in bfModel.toDestroy)
		{
			if (o != null)
			{
				if (bfModel.blacklist.indexOf(o) == -1)
				{
					o.destroy(true);
					o = null;
				}
			}
		}
		bfModel.toDestroy = new Vector.<Object>();
	}
	*/

	//BattleController(Main.osgi.getService(IBattleController)).destroy();
	//StatisticsModel(Main.osgi.getService(IBattlefieldGUI)).objectUnloaded(null);

	//Network(Main.osgi.getService(INetworker)).removeListener((Main.osgi.getService(IBattleController) as BattleController));

	//ChatModel(Main.osgi.getService(IChatBattle)).objectUnloaded(null);

	//WeaponsManager.destroy();


	/*
	CTFModel *ctfModel = Service::getCTFModel();

	if (ctfModel != nullptr)
	{
		ctfModel->objectUnloaded(nullptr);
		Service::setCTFModel(nullptr);
		//Main.osgi.unregisterService(ICaptureTheFlagModelBase);
	}

	DOMModel *domModel = Service::getDOMModel();

	if (domModel != nullptr)
	{
		domModel->objectUnloaded(nullptr);
		Service::setDOMModel(nullptr);
		//Main.osgi.unregisterService(IDOMModel);
	}
	*/

	//IModelService modelsService = IModelService(Main.osgi.getService(IModelService));
	//var inventoryModel:InventoryModel = InventoryModel(modelsService.getModelsByInterface(IInventory)[0]);
	//if (inventoryModel != nullptr)
	//{
	//	inventoryModel.objectUnloaded(null);
	//}

	BattleController *battleController = Service::getBattleController();
	if (battleController == nullptr)
		return;

	delete battleController;


	this->isInBattle = false;
	if (sendToServer == true)
		this->networker->send("battle;i_exit_from_battle");
}

void PanelModel::setCurrentGarage()
{
	this->isInBattle = false;
	this->isBattleSelect = false;
	this->isGarageSelect = true;
}

void PanelModel::setCurrentBattles()
{
	this->isInBattle = false;
	this->isBattleSelect = true;
	this->isGarageSelect = false;
}

void PanelModel::setCurrentBattle()
{
	this->isInBattle = true;
	this->isBattleSelect = false;
	this->isGarageSelect = false;
}


void PanelModel::onButtonBarButtonClick(MainButtonBarEvents e)
{
	this->lock();
	//(Main.osgi.getService(IAchievementModel) as AchievementModel).changeSwitchPanelStateStart(e.typeButton);
	switch (e)
	{
		case MainButtonBarEvents::BATTLE:
			this->showBattleSelect(this->clientObject);
			this->isBattleSelect = true;
			this->lock();
			break;
		case MainButtonBarEvents::BUGS:
			break;
		case MainButtonBarEvents::CLOSE:
			//this->showQuitDialog();
			//this->unlock();
			break;
		case MainButtonBarEvents::GARAGE:
			this->showGarage();
			this->isGarageSelect = true;
			this->lock();
			break;
		case MainButtonBarEvents::HELP:
			//this->showHelpers();
			//this->unlock();
			break;
		case MainButtonBarEvents::SETTINGS:
			/*
			if (this->settingsWindow == null)
				this->showProfile(this->clientObject);
			else
				this->unlock();
			this->unlock();
			*/
			break;
		case MainButtonBarEvents::SOUND:
			//this->togleSoundMute();
			//this->unlock();
			break;
		case MainButtonBarEvents::STAT:
			//this->showTop();
			//this->isTopSelect = true;
			this->unlock();
			break;
		case MainButtonBarEvents::ADDMONEY:
			this->unlock();
			//navigateToURL(new URLRequest("http://mytankspay.net"), "_self");
			break;
		case MainButtonBarEvents::RATING:
			//this->networker->send("lobby;get_rating");
			//(Main.osgi.getService(IGTanksLoader) as GTanksLoaderWindow).setProgress(300);
			break;
		case MainButtonBarEvents::CHALLENGE:
			this->networker->send("lobby;show_quests");
			break;
		case MainButtonBarEvents::FRIENDS:
			//this->networker->send("lobby;get_friends");
			break;
		case MainButtonBarEvents::SOCIALNETS:
			break;
		case MainButtonBarEvents::CLANS:
			break;
		default:
			break;
	}
}

void PanelModel::clickExit()
{
	if (isInBattle == true)
	{
		Lobby *lobby = Service::getLobby();
		if (lobby == nullptr)
			return;

		Service::getPanelModel()->onExitFromBattle();
		lobby->getBattleSelect()->getDataInitBattleSelect();
	}
}

void PanelModel::clickBattles()
{
	Lobby *lobby = Service::getLobby();
	if (lobby == nullptr)
		return;

	if (this->isInBattle == true) {
		this->isInBattle = false;
		Service::getPanelModel()->onExitFromBattle();
		lobby->getBattleSelect()->getDataInitBattleSelect();
	} else if (this->isGarageSelect == true) {
		this->isGarageSelect = false;
		lobby->getBattleSelect()->getDataInitBattleSelect();
	}

	//this->onButtonBarButtonClick(MainButtonBarEvents::BATTLE);
}

void PanelModel::clickGarage()
{
	this->showGarage();
	this->isGarageSelect = true;
	this->lock();
	//this->onButtonBarButtonClick(MainButtonBarEvents::GARAGE);
}

void PanelModel::lock()
{
	//this->mainPanel->mouseEnabled = false;
	//this->mainPanel->mouseChildren = false;
}

void PanelModel::unlock()
{
	//this->mainPanel->mouseEnabled = true;
	//this->mainPanel->mouseChildren = true;
}


void PanelModel::showGarage()
{
	//var battle:BattleSelectModel;
	//(Main.osgi.getService(IGTanksLoader) as GTanksLoaderWindow).unlockLoaderWindow();
	//(Main.osgi.getService(IGTanksLoader) as GTanksLoaderWindow).addProgress(99);

	if (!this->isInBattle)
	{
		this->networker->send("lobby;get_garage_data");
	}


	{
		if (this->isBattleSelect)
		{
			BattleSelectModel *battleSelect = Service::getLobby()->getBattleSelect();
			battleSelect->objectUnloaded(nullptr);
			this->isBattleSelect = false;
			//Main.osgi.unregisterService(IBattleSelectModelBase);
		}
		else
		{
			if (this->isInBattle)
			{
				this->onExitFromBattle();
				this->networker->send("lobby;get_garage_data");
				this->isInBattle = false;
			}
		}
	}
}


void PanelModel::showBattleSelect(ClientObject*, bool sendToServer)
{
	//(Main.osgi.getService(IGTanksLoader) as GTanksLoaderWindow).addProgress(99);
	this->isBattleSelect = true;
	if (this->isGarageSelect == true)
	{
		//GarageModel(Main.osgi.getService(IGarage)).objectUnloaded(null);
		//GarageModel(Main.osgi.getService(IGarage)).kostil = false;
		//Main.osgi.unregisterService(IGarage);
		this->isGarageSelect = false;
	}
	else
	{
		/*
		if (this->isTopSelect)
		{
			TopModel(Main.osgi.getService(ITopModelBase)).closeTop(null);
			this->isTopSelect = false;
		}
		else
		*/
		{
			if (this->isInBattle)
			{
				this->onExitFromBattle(sendToServer);
				this->isInBattle = false;
			}
		}
	}
	this->networker->send("lobby;get_data_init_battle_select");
}
