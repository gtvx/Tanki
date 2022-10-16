#ifndef SERVERINVENTORYMODEL_H
#define SERVERINVENTORYMODEL_H

#include <QVector>

class Array;
class String;
class ClientObject;
class ServerInventoryData;
class BattleEffect;
class EffectsVisualizationModel;
class InventoryModel;
class InventoryItemModel;
class ClientObjectUser;

class ServerInventoryModel
{
	InventoryModel *inventoryModel;
	InventoryItemModel *inventoryItemModel;
	EffectsVisualizationModel *effectModel;
	//private var modelsService:IModelService;
	//private var _objects:Dictionary;
public:
	ServerInventoryModel();

	void init(EffectsVisualizationModel *effectModel);
	void initInventory(QVector<ServerInventoryData*> &items);
	void updateInventory(QVector<ServerInventoryData*> &items);
	void activateItem(const QString &id);
	void enableEffects(ClientObject *clientObject, QVector<BattleEffect*> &effects);
	void enableEffect(ClientObjectUser *clientObject, int itemIndex, int duration);
	void disnableEffect(ClientObjectUser *clientObject, int itemIndex);
	void localTankKilled();
};

#endif // SERVERINVENTORYMODEL_H
