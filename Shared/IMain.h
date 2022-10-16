#ifndef IMAIN_H
#define IMAIN_H

#include <stdint.h>

class QString;
class BattleClient;
class ShowBattleInfo;
class UserInfoClient;
class Tank;
class BattleChatMessage;
class GarageModel;
class QByteArray;
class Network;


class IMain
{
public:
	virtual void draw() = 0;
	virtual void setNick(const QString &nick) = 0;
	virtual void setRank(int rank) = 0;
	virtual void setCrystal(int crystal) = 0;
	virtual void clearBattles() = 0;
	virtual void addBattle(const BattleClient&) = 0;
	virtual void showBattleInfo(const ShowBattleInfo&) = 0;
	virtual void addPlayerToBattle(const QString &battleId, const UserInfoClient&) = 0;
	virtual void removePlayerFromBattle(const QString &battleId, const QString &id) = 0;
	virtual void updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople) = 0;
	virtual void updateCountUsersInDmBattle(const QString &battleId, int people) = 0;
	virtual void removeBattle(const QString &battleId) = 0;
	virtual void showLobby() = 0;
	virtual void hideLobby() = 0;
	virtual void showBattle() = 0;
	virtual void hideBattle() = 0;
	virtual void getSize(int *width, int *height) = 0;
	virtual void initTank(Tank*) = 0;
	virtual void deleteTank(Tank*) = 0;
	virtual void createDamageEffect(Tank *tank, int damage, uint32_t color) = 0;
	virtual bool getBotEnable() = 0;
	virtual void showSuicideIndicator() = 0;
	virtual void hideSuicideIndicator() = 0;
	virtual void battleChatMessage(const BattleChatMessage &message) = 0;
	virtual void socketConnected() = 0;
	virtual void socketDisconnected() = 0;
	virtual void noConnection() = 0;
	virtual void wrongPassword() = 0;
	virtual void captcha() = 0;
	virtual void ban() = 0;	
	virtual void lobbyShowGarage(GarageModel *garageModel) = 0;
	virtual void log(const QString&) = 0;
	virtual bool renderEnable() = 0;
	virtual void setFps(uint32_t) = 0;
	virtual void clearFps() = 0;
	virtual void registrationCaptcha(const QByteArray &data) = 0;
	virtual void showRegistration(Network *network) = 0;
	virtual void hideRegistration() = 0;
	virtual void registrationNotExist() = 0;
	virtual void registrationIncorrect() = 0;
	virtual void registrationNicknameExist() = 0;
	virtual void registrationNicknameNotMatchPattern() = 0;
};

#endif // IMAIN_H
