#include "no_gui_framelobby.h"
#include "projects/tanks/client/battleselect/types/BattleClient.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "projects/tanks/client/battleservice/model/team/UserInfoClient.h"
#include "service.h"
#include "lobby.h"
#include "alternativa/tanks/model/battleselectmodel.h"
#include "projects/tanks/client/battleselect/types/showbattleinfo.h"
#include "enterbattle.h"


class BattleItem
{
	BattleClient battle;
	bool enable;


public:
	BattleItem(const BattleClient &battle, bool enable)
	{
		this->battle = battle;
		this->enable = enable;
	}

	const QString& getId()
	{
		return battle.battleId;
	}

	const QString& getName()
	{
		return battle.name;
	}

	bool isEnable()
	{
		return enable;
	}

	bool isTeam()
	{
		return battle.team;
	}

	void setPeople(int people)
	{
		battle.countPeople = people;
	}

	void setPeople(int redPeople, int bluePeople)
	{
		battle.countRedPeople = redPeople;
		battle.countBluePeople = bluePeople;
	}
};

class BattleUserItem
{
	QString id;
	QString name;
	BattleTeamType type;
	int rank;
	int kills;

public:

	BattleUserItem(const UserInfoClient *user)
	{
		if (user == nullptr) {
			kills = -1;
			return;
		}
		this->id = user->id;
		this->name = user->name;
		this->type = user->type;
		this->rank = user->rank;
		this->kills = user->kills;
	}

	bool isEmpty()
	{
		return kills == -1;
	}

	const QString& getId()
	{
		return id;
	}
};





NoGuiFrameLobby::NoGuiFrameLobby()
{
	battle_enable = false;
	rank = 0;
	currentBattleType = BattleType::DM;
}

NoGuiFrameLobby::~NoGuiFrameLobby()
{

}


void NoGuiFrameLobby::setRank(int rank)
{
	this->rank = rank;
}

void NoGuiFrameLobby::battleEntrance(EnterBattle *enterBattle)
{
	this->_enterBattle = enterBattle;
}

void NoGuiFrameLobby::clearBattles()
{
	battles.clear();
}



void NoGuiFrameLobby::addBattle(const BattleClient &battle)
{
	bool enable = battle.minRank <= rank && battle.maxRank >= rank;
	battles.append(QSharedPointer<BattleItem> (new BattleItem(battle, enable)));

	if (enable == true && !_enterBattle->getName().isEmpty()) {
		Lobby *lobby = Service::getLobby();
		if (lobby == nullptr)
			return;
		if (battle.name == _enterBattle->getName()) {
			lobby->getBattleSelect()->selectBattle(battle.battleId);
		}
	}
}


void NoGuiFrameLobby::showBattleInfo(const ShowBattleInfo &b)
{
	this->reds.clear();
	this->blues.clear();

	currentBattleId = b.battleId;
	currentBattleName = b.name;
	currentBattleType = b.type;
	maxPeople = b.maxPeople;
	battle_enable = b.minRank <= rank && b.maxRank >= rank;
	currentPeopleRedGreen = 0;
	currentPeopleBlue = 0;


	for (const UserInfoClient &user : b.users)
	{
		if (user.type == BattleTeamType::BLUE) {
			blues.append(QSharedPointer<BattleUserItem>(new BattleUserItem(&user)));
			currentPeopleBlue++;
		} else {
			reds.append(QSharedPointer<BattleUserItem>(new BattleUserItem(&user)));
			currentPeopleRedGreen++;
		}
	}

	bool free_green_red = false;
	bool free_blue = false;

	if (b.maxPeople > this->reds.length()) {
		free_green_red = true;
	}

	if (b.type != BattleType::DM) {
		if (b.maxPeople > this->blues.length()) {
			free_blue = true;
		}
	}

	if (!_enterBattle->getName().isEmpty() && _enterBattle->getName() == currentBattleName) {
		autoEntrance(free_green_red, free_blue);
	}
}



void NoGuiFrameLobby::addPlayerToBattle(const QString &battleId, const UserInfoClient &user)
{
	if (currentBattleId != battleId)
		return;
	if (user.type == BattleTeamType::BLUE) {
		currentPeopleBlue++;
		this->blues.append(QSharedPointer<BattleUserItem>(new BattleUserItem(&user)));
	} else {
		currentPeopleRedGreen++;
		this->reds.append(QSharedPointer<BattleUserItem>(new BattleUserItem(&user)));
	}
}



void NoGuiFrameLobby::removePlayerFromBattle(const QString &battleId, const QString &id)
{
	if (currentBattleId != battleId)
		return;

	for (int i = 0; i < this->reds.length();) {
		BattleUserItem *item = this->reds.at(i).get();
		if (item->getId() == id) {
			currentPeopleRedGreen--;
			delete item;
			continue;
		}
		i++;
	}

	for (int i = 0; i < this->blues.length();) {
		BattleUserItem *item = this->blues.at(i).get();
		if (item->getId() == id) {
			currentPeopleBlue--;
			this->blues.removeAt(i);
			continue;
		}
		i++;
	}

	bool free_green_red = false;
	bool free_blue = false;

	if (maxPeople > this->reds.length()) {
		free_green_red = true;
	}

	if (currentBattleType != BattleType::DM) {
		if (maxPeople > this->blues.length()) {
			free_blue = true;
		}
	}

	if (!_enterBattle->getName().isEmpty() && _enterBattle->getName() == currentBattleName) {
		autoEntrance(free_green_red, free_blue);
	}
}


void NoGuiFrameLobby::updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople)
{
	for (int i = 0; i < battles.length(); i++) {
		BattleItem *item = battles.at(i).get();
		if (item->isTeam() == true && item->getId() == battleId) {
			item->setPeople(redPeople, bluePeople);
			break;
		}
	}
}

void NoGuiFrameLobby::updateCountUsersInDmBattle(const QString &battleId, int people)
{
	for (int i = 0; i < battles.length(); i++) {
		BattleItem *item = battles.at(i).get();
		if (item->isTeam() == false && item->getId() == battleId) {
			item->setPeople(people);
			break;
		}
	}
}



void NoGuiFrameLobby::removeBattle(const QString &battleId)
{
	for (int i = 0; i < battles.length(); i++) {
		BattleItem *item = battles.at(i).get();
		if (item->getId() == battleId) {
			delete item;
			if (currentBattleId == battleId) {
				reds.clear();
				blues.clear();
			}
			currentBattleId.clear();
			currentBattleName.clear();
			break;
		}
	}
}


void NoGuiFrameLobby::on_pushButton_play_red_clicked()
{
	if (battle_enable == false)
		return;

	if (maxPeople <= currentPeopleRedGreen)
		return;

	Lobby *lobby = Service::getLobby();
	if (lobby == nullptr)
		return;

	BattleSelectModel *battleSelectModel = lobby->getBattleSelect();
	if (battleSelectModel == nullptr)
		return;

	battleSelectModel->startLoadBattle();

	if (currentBattleType == BattleType::DM) {
		battleSelectModel->fightBattle(currentBattleId, false, false);
	} else {
		battleSelectModel->fightBattle(currentBattleId, true, true);
	}
}


void NoGuiFrameLobby::on_pushButton_play_blue_clicked()
{
	if (battle_enable == false)
		return;
	if (currentBattleType == BattleType::DM)
		return;
	if (maxPeople <= currentPeopleBlue)
		return;
	Lobby *lobby = Service::getLobby();
	if (lobby == nullptr)
		return;

	BattleSelectModel *battleSelectModel = lobby->getBattleSelect();
	if (battleSelectModel == nullptr)
		return;

	battleSelectModel->startLoadBattle();
	battleSelectModel->fightBattle(currentBattleId, true, false);
}



void NoGuiFrameLobby::autoEntrance(bool free_green_red, bool free_blue)
{
	bool ok = false;
	if (free_green_red == true) {
		if (_enterBattle->isTeam()) {
			if (currentBattleType == BattleType::DM) {
				if (_enterBattle->getTeam() == BattleTeamType::NONE) {
					ok = true;
					on_pushButton_play_red_clicked();
				}
			} else {
				if (_enterBattle->getTeam() == BattleTeamType::RED) {
					ok = true;
					on_pushButton_play_red_clicked();
				}
			}
		} else {
			ok = true;
			on_pushButton_play_red_clicked();
		}

	}
	if (ok == false && free_blue == true) {
		if (_enterBattle->isTeam()) {
			if (_enterBattle->getTeam() == BattleTeamType::BLUE) {
				on_pushButton_play_blue_clicked();
			}
		} else {
			on_pushButton_play_blue_clicked();
		}
	}
}
