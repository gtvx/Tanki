#ifndef SERVICE_H
#define SERVICE_H

typedef unsigned short  uint16_t;
class ITank;
class ICaptureTheFlagModelBase;
class INetworker;
class Network;
class IWeaponCommonModel;
class WeaponCommonModel;
class BattleController;
class IBattleField;
class BattlefieldModel;
class PanelModel;
class QString;
class IMain;
class TankModel;
class Authorization;
class Lobby;
class CTFModel;
class DOMModel;
class IMaterialRegistry;
class IFlamethrower;


class Service
{
public:
	Service();

	static void setTankModel(TankModel*);
	static TankModel* getTankModel();
	static ITank* getITank();

	static void setICaptureTheFlagModelBase(ICaptureTheFlagModelBase*);
	static ICaptureTheFlagModelBase *getICaptureTheFlagModelBase();

	static void setNetworker(Network*);
	static Network* getNetworker();

	static void setIWeaponCommonModel(IWeaponCommonModel*);
	static IWeaponCommonModel* getIWeaponCommonModel();

	static void setWeaponCommonModel(WeaponCommonModel*);
	static WeaponCommonModel* getWeaponCommonModel();

	static void setBattleController(BattleController*);
	static BattleController* getBattleController();

	static void setIBattleField(IBattleField*);
	static IBattleField* getIBattleField();

	static void setBattlefieldModel(BattlefieldModel*);
	static BattlefieldModel* getBattlefieldModel();

	static void setPanelModel(PanelModel*);
	static PanelModel* getPanelModel();

	static void setResourcesPath(const QString&);
	static QString getResourcesPath();

	static void setClientPath(const QString&);
	static QString getClientPath();

	static void setIMain(IMain*);
	static IMain* getIMain();

	static void setHost(const QString &host);
	static QString getHost();

	static void setPort(uint16_t poit);
	static uint16_t getPort();

	static void setAuthorization(Authorization*);
	static Authorization* getAuthorization();

	static void setLobby(Lobby*);
	static Lobby* getLobby();

	static void setCTFModel(CTFModel*);
	static CTFModel* getCTFModel();

	static void setDOMModel(DOMModel*);
	static DOMModel* getDOMModel();

	static void setIMaterialRegistry(IMaterialRegistry *materialRegistry);
	static IMaterialRegistry* getIMaterialRegistry();

	static void setIFlamethrower(IFlamethrower*);
	static IFlamethrower* getIFlamethrower();
};

#endif // SERVICE_H
