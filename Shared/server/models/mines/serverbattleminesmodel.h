#ifndef SERVERBATTLEMINESMODEL_H
#define SERVERBATTLEMINESMODEL_H

class BattleMinesModel;
class QString;
class QByteArray;
class ClientObjectUser;

class ServerBattleMinesModel
{	
	BattleMinesModel *model;

public:
	ServerBattleMinesModel(BattleMinesModel*);

	void init();
	void initModel(const QByteArray &data);
	void initMines(const QByteArray &data);
	void removeMines(const QString &ownerId);
	void putMine(ClientObjectUser *ownerObject, const QString &mineId, const QString &userId, double x, double y, double z);
	void activateMine(const QString &ownerId);
	void hitMine(ClientObjectUser *ownerObject, const QString &mineId);
};

#endif // SERVERBATTLEMINESMODEL_H
