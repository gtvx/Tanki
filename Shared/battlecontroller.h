#ifndef BATTLECONTROLLER_H
#define BATTLECONTROLLER_H

#include "networking/INetworkListener.h"
#include <qglobal.h>
#include <memory>

class QString;
class ClientObjectUser;
class ClientObject;
class BattlefieldModel;
class IWeaponController;
class ClientObjectWeapon;
class FlamethrowerModel;
class SmokyModel;
class Network;
class TankModel;
class ClientObjectCTF;
class CTFModel;
class WeaponModelEmpty;
class BotModel;
class DOMModel;
class ClientObjectBonus;
class BattleMinesModel;
class ServerBattleMinesModel;
class PlasmaModel;
class ThunderModel;
class ServerInventoryModel;
class EffectsVisualizationModel;
class HealingGunModel;
class ShaftModel;
class FreezeModel;
class RailgunModel;
class RicochetModel;
class TerminatorModel;



class BattleController : public INetworkListener
{
	ClientObject *client;
	BattlefieldModel *battle;
	Network *network;
	TankModel *tankModel;
	ClientObjectCTF *ctfObj;
	CTFModel *ctfModel;
	DOMModel *domModel;
	BotModel *botModel;
	BattleMinesModel *battleMinesModel;
	ServerBattleMinesModel *serverMinesModel;
	QHash<QString, ClientObjectBonus*> *clientObjectBonuses;
	bool battle_init;
	bool onData(Command*);
	void initTank(const QByteArray &js);
	void initBattle(const QByteArray &js);
	void initLocalClientObject(const QString &id, const QString &name);
	void parseSpawnCommand(const QByteArray &js);
	void parseShotsData(const QByteArray &js);
	void moveTank(const QByteArray &js);
	void removeUser(const QString &id);
	void parseFinishBattle(const QByteArray &json);
	void parseFire(ClientObjectUser *user, const QByteArray &js);
	void parseChangeSpecTank(ClientObjectUser *client, const QByteArray &json);
	void createDamageEffect(ClientObjectUser *user, int damage, uint32_t color);
	void parseCTF(const QByteArray &js);
	void flagDrop(const QByteArray &js);
	void returnFlag(const QString &tankId, const QString &team);
	void deliverFlag(const QString &tankId, const QString &team);
	void parseHP(ClientObjectUser *user, double damage, int color_id);
	void moveAnticheatTank(const QByteArray &js);
	void onChatMessage(const QByteArray &js);
	void onChatMessageSpectator(const QString &text);
	void moveBot(const QByteArray &js);
	void parseDOM(const QByteArray &js);
	void parseSpawnBonus(const QByteArray &js);
	void putMine(const QByteArray &js);
	void parseStartFire(ClientObjectUser *user, const QString &firing, const QByteArray &argsJSON);
	void parseStopFire(ClientObjectUser *user, const QString &firing);
	void parseStartFireTwins(ClientObjectUser *user, const QByteArray &json);
	void parseShaftQuickFire(const QString &userName, const QByteArray &json);
	void movementControl(const QByteArray &json);
	void rotateTurret(const QByteArray &json);


public:
	WeaponModelEmpty *weaponModelEmpty;
	SmokyModel *smokyModel;
	FlamethrowerModel *flamethrowerModel;
	PlasmaModel *twinsModel;
	ThunderModel *thunderModel;
	ServerInventoryModel *serverInventoryModel;
	EffectsVisualizationModel *effectsModel;
	HealingGunModel *isidaModel;
	ShaftModel *shaftModel;
	FreezeModel *frezeeModel;
	RailgunModel *railgunModel;
	RicochetModel *ricochetModel;
	TerminatorModel *terminatorModel;



	QHash<QString, ClientObjectUser*> *tanks;

	BattleController(Network *network);
	virtual ~BattleController();
	std::shared_ptr<ClientObjectUser> initClientObject(const QString &id, const QString &name);
	IWeaponController* getWeaponController(ClientObjectWeapon *obj);
	void destroy();
};

#endif // BATTLECONTROLLER_H
