#include "serverinventorymodel.h"
#include "alternativa/tanks/models/effectsvisualization/effectsvisualizationmodel.h"
#include "alternativa/object/clientobjectuser.h"


ServerInventoryModel::ServerInventoryModel()
{

}

void ServerInventoryModel::init(EffectsVisualizationModel *effectModel)
{
	//this->modelsService = IModelService(Main.osgi.getService(IModelService));
	//this->inventoryModel = InventoryModel(this->modelsService.getModelsByInterface(IInventory)[0]);
	//this->inventoryItemModel = InventoryItemModel(this->modelsService.getModelsByInterface(IInventoryItemModel)[0]);
	this->effectModel = effectModel;
	//this->_objects = new Dictionary();
}

void ServerInventoryModel::initInventory(QVector<ServerInventoryData*> &items)
{
	//var data:ServerInventoryData;
	//this->inventoryModel->objectLoaded(nullptr);
	for (ServerInventoryData *data : items)
	{
		(void)data;
		//ClientObject *clientObject = this->getClientObject(data.id);
		//this->inventoryItemModel->initObject(clientObject, null, data.count, data.itemEffectTime, data.slotId, data.itemRestSec);
		//this->_objects[data.id] = clientObject;
	}
}

void ServerInventoryModel::updateInventory(QVector<ServerInventoryData*> &items)
{
	if (((this->inventoryModel == nullptr) || (this->inventoryItemModel == nullptr)))
	{
		return;
	}
	for (ServerInventoryData *data : items)
	{
		(void)data;
		//this->inventoryItemModel->updateItemCount(this->_objects[data.id], data.count);
	}
}

void ServerInventoryModel::activateItem(const QString &id)
{
	(void)id;
	//this->inventoryItemModel->activated(this->_objects[id]);
}

void ServerInventoryModel::enableEffects(ClientObject *clientObject, QVector<BattleEffect*> &effects)
{
	this->effectModel->initObject(clientObject, effects);
}

void ServerInventoryModel::enableEffect(ClientObjectUser *clientObject, int itemIndex, int duration)
{
	if (clientObject == nullptr)
		return;
	this->effectModel->effectActivated(clientObject, clientObject->getId(), itemIndex, duration);
}

void ServerInventoryModel::disnableEffect(ClientObjectUser *clientObject, int itemIndex)
{
	if (clientObject == nullptr)
		return;
	this->effectModel->effectStopped(clientObject, clientObject->getId(), itemIndex);
}

void ServerInventoryModel::localTankKilled()
{
	/*
	if (this->inventoryModel == nullptr)
	{
		return;
	}
	this->inventoryModel.killCurrentUser(nullptr);
	*/
}
