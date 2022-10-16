#include "framelobby.h"
#include "ui_framelobby.h"
#include "projects/tanks/client/battleselect/types/BattleClient.h"
#include "projects/tanks/client/battleservice/model/team/BattleTeamType.h"
#include "projects/tanks/client/battleservice/model/team/UserInfoClient.h"
#include "service.h"
#include "lobby.h"
#include "alternativa/tanks/model/battleselectmodel.h"
#include "projects/tanks/client/battleselect/types/showbattleinfo.h"
#include "enterbattle.h"
#include <QDebug>


class BattleItem : public QTreeWidgetItem
{
	BattleClient battle;
	bool enable;


	void setData(int column, int role, const QVariant &value)
	{
		if (role == Qt::ItemIsEditable)
			return;
		QTreeWidgetItem::setData(column, role, value);
	}

public:
	BattleItem(const BattleClient &battle, bool enable)
	{
		this->battle = battle;
		this->enable = enable;
		this->setFlags(flags() | Qt::ItemIsEditable);

		setFont(0, QFont("", 9, QFont::Bold));
		if (enable == false) {
			setForeground(0, QBrush(QColor(200, 200, 200)));
		} else {
		}

		this->setText(0, battle.name);
		if (battle.team == false)
			setPeople(battle.countPeople);
		else
			setPeople(battle.countRedPeople, battle.countBluePeople);
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
		this->setText(1, QString::number(people));
	}

	void setPeople(int redPeople, int bluePeople)
	{
		battle.countRedPeople = redPeople;
		battle.countBluePeople = bluePeople;
		this->setText(1, QString::number(redPeople) + "/" + QString::number(bluePeople));
	}
};

class BattleUserItem : public QTreeWidgetItem
{
	QString id;
	QString name;
	BattleTeamType type;
	int rank;
	int kills;

public:

	BattleUserItem(QTreeWidget *tree, const UserInfoClient *user) :
		QTreeWidgetItem(tree)
	{
		if (user == nullptr) {
			setText(0, "-");
			kills = -1;
			return;
		}
		this->id = user->id;
		this->name = user->name;
		this->type = user->type;
		this->rank = user->rank;
		this->kills = user->kills;
		setText(0, name);
		setText(1, QString::number(kills));
	}

	bool operator <(const QTreeWidgetItem& other) const
	{
		const BattleUserItem &_other = (const BattleUserItem&)other;
		return this->kills > _other.kills;
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





FrameLobby::FrameLobby(QWidget *parent) :
	QFrame(parent),
	ui(new Ui::FrameLobby)
{
	ui->setupUi(this);

	battle_enable = false;
	rank = 0;
	currentBattleType = BattleType::DM;

	ui->treeWidget_battles->setColumnWidth(0, 250);
	ui->treeWidget_blues->setColumnWidth(0, 130);
	ui->treeWidget_reds->setColumnWidth(0, 130);
	ui->treeWidget_blues->setColumnWidth(1, 30);
	ui->treeWidget_reds->setColumnWidth(1, 30);
}

FrameLobby::~FrameLobby()
{
	delete ui;
}


void FrameLobby::setRank(int rank)
{
	this->rank = rank;
}

void FrameLobby::battleEntrance(EnterBattle *enterBattle)
{
	this->_enterBattle = enterBattle;
}

void FrameLobby::clearBattles()
{
	ui->treeWidget_battles->clear();
}



void FrameLobby::addBattle(const BattleClient &battle)
{
	bool enable = battle.minRank <= rank && battle.maxRank >= rank;
	ui->treeWidget_battles->addTopLevelItem(new BattleItem(battle, enable));

	if (enable == true && !_enterBattle->getName().isEmpty()) {
		Lobby *lobby = Service::getLobby();
		if (lobby == nullptr)
			return;
		if (battle.name == _enterBattle->getName()) {
			lobby->getBattleSelect()->selectBattle(battle.battleId);
		}
	}
}


void FrameLobby::showBattleInfo(const ShowBattleInfo &b)
{
	ui->treeWidget_reds->clear();
	ui->treeWidget_blues->clear();
	ui->lineEdit_battle_id->setText(b.battleId);
	ui->label_battle_name->setText(b.name);
	ui->label_battle_type->setText(BattleType_toString(b.type));

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
			new BattleUserItem(ui->treeWidget_blues, &user);
			currentPeopleBlue++;
		} else {
			new BattleUserItem(ui->treeWidget_reds, &user);
			currentPeopleRedGreen++;
		}
	}

	bool free_green_red = false;
	bool free_blue = false;

	while (b.maxPeople > ui->treeWidget_reds->topLevelItemCount()) {
		auto item = new BattleUserItem(ui->treeWidget_reds, nullptr);
		item->setText(0, "-");
		free_green_red = true;
	}

	if (b.type != BattleType::DM) {
		while (b.maxPeople > ui->treeWidget_blues->topLevelItemCount()) {
			auto item = new BattleUserItem(ui->treeWidget_blues, nullptr);
			item->setText(0, "-");
			free_blue = true;
		}
	}

	ui->treeWidget_reds->sortItems(1, Qt::AscendingOrder);
	ui->treeWidget_blues->sortItems(1, Qt::AscendingOrder);


	if (!_enterBattle->getName().isEmpty() && _enterBattle->getName() == currentBattleName) {
		autoEntrance(free_green_red, free_blue);
	}
}

void FrameLobby::autoEntrance(bool free_green_red, bool free_blue)
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

void FrameLobby::on_treeWidget_battles_itemSelectionChanged()
{
	Lobby *lobby = Service::getLobby();
	if (lobby == nullptr)
		return;

	BattleItem *item = (BattleItem*)ui->treeWidget_battles->currentItem();
	lobby->getBattleSelect()->selectBattle(item->getId());
}


void FrameLobby::addPlayerToBattle(const QString &battleId, const UserInfoClient &user)
{
	if (currentBattleId != battleId)
		return;
	if (user.type == BattleTeamType::BLUE) {
		currentPeopleBlue++;
		new BattleUserItem(ui->treeWidget_blues, &user);
		ui->treeWidget_blues->sortItems(1, Qt::AscendingOrder);
		BattleUserItem *last = (BattleUserItem*)ui->treeWidget_blues->topLevelItem(ui->treeWidget_blues->topLevelItemCount()-1);
		if (last->isEmpty())
			delete last;
	} else {
		currentPeopleRedGreen++;
		new BattleUserItem(ui->treeWidget_reds, &user);
		ui->treeWidget_reds->sortItems(1, Qt::AscendingOrder);
		BattleUserItem *last = (BattleUserItem*)ui->treeWidget_reds->topLevelItem(ui->treeWidget_reds->topLevelItemCount()-1);
		if (last->isEmpty())
			delete last;
	}
}



void FrameLobby::removePlayerFromBattle(const QString &battleId, const QString &id)
{
	if (currentBattleId != battleId)
		return;

	for (int i = 0; i < ui->treeWidget_reds->topLevelItemCount();) {
		BattleUserItem *item = (BattleUserItem*)ui->treeWidget_reds->topLevelItem(i);
		if (item->getId() == id) {
			currentPeopleRedGreen--;
			delete item;
			continue;
		}
		i++;
	}

	for (int i = 0; i < ui->treeWidget_blues->topLevelItemCount();) {
		BattleUserItem *item = (BattleUserItem*)ui->treeWidget_blues->topLevelItem(i);
		if (item->getId() == id) {
			currentPeopleBlue--;
			delete item;
			continue;
		}
		i++;
	}

	bool free_green_red = false;
	bool free_blue = false;

	while (maxPeople > ui->treeWidget_reds->topLevelItemCount()) {
		auto item = new BattleUserItem(ui->treeWidget_reds, nullptr);
		item->setText(0, "-");
		free_green_red = true;
	}

	if (currentBattleType != BattleType::DM) {
		while (maxPeople > ui->treeWidget_blues->topLevelItemCount()) {
			auto item = new BattleUserItem(ui->treeWidget_blues, nullptr);
			item->setText(0, "-");
			free_blue = true;
		}
	}

	if (!_enterBattle->getName().isEmpty() &&  _enterBattle->getName() == currentBattleName) {
		autoEntrance(free_green_red, free_blue);
	}
}


void FrameLobby::updateCountUsersInTeamBattle(const QString &battleId, int redPeople, int bluePeople)
{
	for (int i = 0; i < ui->treeWidget_battles->topLevelItemCount(); i++) {
		BattleItem *item = (BattleItem*)ui->treeWidget_battles->topLevelItem(i);
		if (item->isTeam() == true && item->getId() == battleId) {
			item->setPeople(redPeople, bluePeople);
			break;
		}
	}
}

void FrameLobby::updateCountUsersInDmBattle(const QString &battleId, int people)
{
	for (int i = 0; i < ui->treeWidget_battles->topLevelItemCount(); i++) {
		BattleItem *item = (BattleItem*)ui->treeWidget_battles->topLevelItem(i);
		if (item->isTeam() == false && item->getId() == battleId) {
			item->setPeople(people);
			break;
		}
	}
}



void FrameLobby::removeBattle(const QString &battleId)
{
	for (int i = 0; i < ui->treeWidget_battles->topLevelItemCount(); i++) {
		BattleItem *item = (BattleItem*)ui->treeWidget_battles->topLevelItem(i);
		if (item->getId() == battleId) {
			delete item;
			if (currentBattleId == battleId) {
				ui->treeWidget_reds->clear();
				ui->treeWidget_blues->clear();
				ui->label_battle_name->clear();
				ui->label_battle_type->clear();
				ui->lineEdit_battle_id->clear();
			}
			currentBattleId.clear();
			currentBattleName.clear();
			break;
		}
	}
}


void FrameLobby::on_pushButton_play_red_clicked()
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


void FrameLobby::on_pushButton_play_blue_clicked()
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
