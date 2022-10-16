#include "service.h"
#include <QString>
#include "alternativa/tanks/models/tank/tankmodel.h"


namespace
{
	QString resources_path;
	QString client_path;
	QString host;
	uint16_t port;

	TankModel *tankmodel;
	ITank *itank;
	ICaptureTheFlagModelBase *icapturetheflagmodelbase;
	Network *networker;
	IWeaponCommonModel *iweaponcommonmodel;
	WeaponCommonModel *weaponcommonmodel;
	BattleController *battlecontroller;
	IBattleField *ibattlefield;
	BattlefieldModel *battlefield;
	PanelModel *panelmodel;
	IMain *imain;
	Authorization *authorization;
	Lobby *lobby;
	CTFModel *ctfmodel;
	DOMModel *dommodel;
	IMaterialRegistry *materialRegistry;
	IFlamethrower *iflamethrower;
}


void Service::setTankModel(TankModel *tankModel)
{
	::itank = tankModel;
	::tankmodel = tankModel;
}

TankModel* Service::getTankModel()
{
	return ::tankmodel;
}

ITank* Service::getITank()
{
	return ::itank;
}

void Service::setICaptureTheFlagModelBase(ICaptureTheFlagModelBase *icapturetheflagmodelbase)
{
	::icapturetheflagmodelbase = icapturetheflagmodelbase;
}

ICaptureTheFlagModelBase* Service::getICaptureTheFlagModelBase()
{
	return ::icapturetheflagmodelbase;
}

void Service::setNetworker(Network *networker)
{
	::networker = networker;
}

Network* Service::getNetworker()
{
	return ::networker;
}

void Service::setIWeaponCommonModel(IWeaponCommonModel *iweaponcommonmodel)
{
	::iweaponcommonmodel = iweaponcommonmodel;
}

IWeaponCommonModel* Service::getIWeaponCommonModel()
{
	return ::iweaponcommonmodel;
}

void Service::setWeaponCommonModel(WeaponCommonModel *weaponcommonmodel)
{
	::weaponcommonmodel = weaponcommonmodel;
}

WeaponCommonModel* Service::getWeaponCommonModel()
{
	return ::weaponcommonmodel;
}

void Service::setBattleController(BattleController *battlecontroller)
{
	::battlecontroller = battlecontroller;
}

BattleController* Service::getBattleController()
{
	return ::battlecontroller;
}

void Service::setIBattleField(IBattleField *ibattlefield)
{
	::ibattlefield = ibattlefield;
}

IBattleField* Service::getIBattleField()
{
	return ::ibattlefield;
}

void Service::setBattlefieldModel(BattlefieldModel *battlefield)
{
	::battlefield = battlefield;
}

BattlefieldModel* Service::getBattlefieldModel()
{
	return ::battlefield;
}


void Service::setPanelModel(PanelModel *panelmodel)
{
	::panelmodel = panelmodel;
}

PanelModel* Service::getPanelModel()
{
	return ::panelmodel;
}

void Service::setResourcesPath(const QString &path)
{
	::resources_path = path;
}

QString Service::getResourcesPath()
{
	return ::resources_path;
}

void Service::setClientPath(const QString &client_path)
{
	::client_path = client_path;
}

QString Service::getClientPath()
{
	return ::client_path;
}

void Service::setIMain(IMain *imain)
{
	::imain = imain;
}

IMain* Service::getIMain()
{
	return ::imain;
}

void Service::setHost(const QString &host)
{
	::host = host;
}

QString Service::getHost()
{
	return ::host;
}

void Service::setPort(uint16_t port)
{
	::port = port;
}

uint16_t Service::getPort()
{
	return ::port;
}

void Service::setAuthorization(Authorization *authorization)
{
	::authorization = authorization;
}

Authorization* Service::getAuthorization()
{
	return ::authorization;
}

void Service::setLobby(Lobby *lobby)
{
	::lobby = lobby;
}

Lobby* Service::getLobby()
{
	return ::lobby;
}

void Service::setCTFModel(CTFModel *ctfmodel)
{
	::ctfmodel = ctfmodel;
}

CTFModel* Service::getCTFModel()
{
	return ::ctfmodel;
}

void Service::setDOMModel(DOMModel *dommodel)
{
	::dommodel = dommodel;
}

DOMModel* Service::getDOMModel()
{
	return ::dommodel;
}

void Service::setIMaterialRegistry(IMaterialRegistry *materialRegistry)
{
	::materialRegistry = materialRegistry;
}

IMaterialRegistry* Service::getIMaterialRegistry()
{
	return ::materialRegistry;
}

void Service::setIFlamethrower(IFlamethrower *iflamethrower)
{
	::iflamethrower = iflamethrower;
}

IFlamethrower *Service::getIFlamethrower()
{
	return ::iflamethrower;
}

