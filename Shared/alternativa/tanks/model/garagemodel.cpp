#include "garagemodel.h"
#include "../gui/garagewindow.h"
#include "forms/garage/partslist.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/model/tankpreview.h"
#include "service.h"
#include "IMain.h"
#include "networking/network.h"
#include "lobby.h"
#include "_global.h"
#include <QTimer>



QHash<QString, std::shared_ptr<ItemParams>> GarageModel::itemsParams;


GarageModel::GarageModel()
{
	kostil = false;
	lockMount = false;

	o = new QObject();

	//_interfaces.push(IModel);
	//_interfaces.push(IGarage);
	//_interfaces.push(IGarageModelBase);
	//_interfaces.push(IObjectLoadListener);
	//_interfaces.push(IItemListener);
	//_interfaces.push(IItemEffectListener);
	//this->layer = Main.contentUILayer;
	//itemsParams = new Dictionary();
	//this->resourceRegister = (Main.osgi.getService(IResourceService) as IResourceService);
	//this->dialogsLayer = ((Main.osgi.getService(IMainContainerService) as IMainContainerService).dialogsLayer as DisplayObjectContainer);



}

void GarageModel::initObject(ClientObject *clientObject, const QString &country, double rate, const Long &garageBoxId, Network *networker)
{
	(void)rate;
	(void)garageBoxId;
	(void)country;
	(void)clientObject;

	this->garageWindow = nullptr;
	//this->garageBoxId = garageBoxId;
	//this->localeService = (Main.osgi.getService(ILocaleService) as ILocaleService);
	//GarageModel.buyCrystalCurrency = "RUR";
	//GarageModel.buyCrystalRate = 3;
	this->socket = networker;
	this->objectLoaded(nullptr);

	Service::getIMain()->lobbyShowGarage(this);
}

void GarageModel::objectLoaded(ClientObject *object)
{
	if (this->kostil == false)
	{
		this->kostil = true;
		//Main.writeVarsToConsoleChannel("GARAGE MODEL", "objectLoaded");
		this->clientObject = object;
		//Main.stage.quality = StageQuality.HIGH;
		//itemsInfo = new Dictionary();
		//this->mountedItems = new Array();
		//this->itemsForMount = new Array();
		//this->modelRegister = (Main.osgi.getService(IModelService) as IModelService);
		//this->itemModel = ((this->modelRegister.getModelsByInterface(IItem) as Vector.<IModel>)[0] as IItem);
		//this->panelModel = (Main.osgi.getService(IPanel) as IPanel);
		this->garageWindow = new GarageWindow(this->garageBoxId);
		//this->helpService = (Main.osgi.getService(IHelpService) as IHelpService);
		//this->storeHelper = new StoreListHelper();
		//this->warehouseHelper = new WarehouseListHelper();
		//this->helpService.registerHelper(this->HELPER_GROUP_KEY, this->HELPER_STORE, this->storeHelper, true);
		//this->helpService.registerHelper(this->HELPER_GROUP_KEY, this->HELPER_WAREHOUSE, this->warehouseHelper, true);
		//Main.stage.addEventListener(Event.RESIZE, this->alignHelpers);
		//this->alignHelpers();
		/*
		Timer *timer = new Timer(6000, 1);
		timer.addEventListener(TimerEvent.TIMER_COMPLETE, function (e:TimerEvent):void
							   {
								   showWindow();
								   panelModel.partSelected(1);
								   (Main.osgi.getService(IGTanksLoader) as GTanksLoaderWindow).setFullAndClose(null);
								   if (Lobby.firstInit)
								   {
									   Network(Main.osgi.getService(INetworker)).send("lobby;user_inited");
									   Lobby.firstInit = false;
								   }
							   });
		timer.start();
		*/


		QTimer::singleShot(6000, o, [this]
		{
			//showWindow();
			//panelModel.partSelected(1);
			//(Main.osgi.getService(IGTanksLoader) as GTanksLoaderWindow).setFullAndClose(null);
			if (Lobby::firstInit == true)
			{
				this->socket->send("lobby;user_inited");
				Lobby::firstInit = false;
			}
		});

	}
}

void GarageModel::objectUnloaded(ClientObject *object)
{
	(void)object;
	/*
	Main.writeVarsToConsoleChannel("GARAGE MODEL", "objectUnloaded");
	var moneyService:IMoneyService = (GarageModelActivator.osgi.getService(IMoneyService) as IMoneyService);
	if (moneyService != null)
	{
		moneyService.removeListener(this);
	}
	this->hideWindow();
	Main.stage.removeEventListener(Event.RESIZE, this->alignHelpers);
	this->helpService.unregisterHelper(this->HELPER_GROUP_KEY, this->HELPER_STORE);
	this->helpService.unregisterHelper(this->HELPER_GROUP_KEY, this->HELPER_WAREHOUSE);
	this->storeHelper = null;
	this->warehouseHelper = null;
	var loaderWindowService:ILoaderWindowService = (Main.osgi.getService(ILoaderWindowService) as ILoaderWindowService);
	loaderWindowService.unlockLoaderWindow();
	this->garageWindow = null;
	this->clientObject = null;
	Main.stage.quality = StageQuality.HIGH;
	*/
}

void GarageModel::initItem(const QString &id, std::shared_ptr<ItemParams> item)
{
	this->items.insert(id, item);
}

void GarageModel::initDepot(ClientObject *clientObject, QVector<std::shared_ptr<ItemInfo>> &itemsOnSklad)
{
	(void)clientObject;


	for (int i = 0; i < itemsOnSklad.length(); i++ )
	{
		std::shared_ptr<ItemInfo> info = itemsOnSklad.at(i);
		QString itemId = info->itemId;
		std::shared_ptr<ItemParams> itemParams = this->items.value(itemId);

		itemsParams[itemId] = itemParams;
		itemsInfo.insert(itemId, info);

		if (itemId.indexOf("HD_") == -1)
		{
			this->garageWindow->addItemToWarehouse(itemId, itemParams, info);
		}
		else
		{
			//this->garageWindow->addSkin(itemId, itemParams);
		}

		/*
		if (this->itemsForMount.indexOf(itemId) != -1)
		{
			this->garageWindow->mountItem(itemId);
			this->itemsForMount->splice(this->itemsForMount.indexOf(itemId), 1);
			this->mountedItems.push(itemId);
			switch (itemParams.itemType)
			{
				case ItemTypeEnum::WEAPON:
					this->mountedWeaponId = itemId;
					mounted[0] = this->mountedWeaponId;
					this->mountedWeaponInfo = info;
					this->mountedWeaponParams = itemParams;
					break;
				case ItemTypeEnum::ARMOR:
					this->mountedArmorId = itemId;
					mounted[1] = this->mountedArmorId;
					this->mountedArmorInfo = info;
					this->mountedArmorParams = itemParams;
					break;
			}
		}
		*/
	}
	//this->garageWindow.addEventListener(GarageWindowEvent.WAREHOUSE_ITEM_SELECTED, this->onWarehouseListSelect);
	//this->garageWindow.addEventListener(GarageWindowEvent.SETUP_ITEM, this->onSetupClick);
	//this->garageWindow.addEventListener(GarageWindowEvent.UPGRADE_ITEM, this->onUpgradeClick);
	//this->garageWindow.addEventListener(GarageWindowEvent.PROCESS_SKIN, this->onSkinClick);
	//this->garageWindow.addEventListener(GarageWindowEvent.ADD_CRYSTALS, this->onBuyCrystalsClick);
	//this->garageWindow.selectFirstItemInWarehouse();
}

ItemParams* GarageModel::getItemParams(const QString &itemId)
{
	return itemsParams.value(itemId).get();
}

void GarageModel::mountItem(ClientObject *clientObject, const QString &itemToUnmountId, const QString &itemToMountId)
{
	(void)clientObject;

	//bool bar3xQuickSet = false;

	if (!itemToUnmountId.isEmpty())
	{
		/*
		int index = this->mountedItems.indexOf(itemToUnmountId);
		if (index != -1)
		{
			if (itemsParams[itemToUnmountId] != null)
			{
				this->garageWindow.unmountItem(itemToUnmountId);
			}
			this->mountedItems.splice(index, 1);
		}
		*/
	}
	else
	{
		//bar3xQuickSet = true;
	}

	ItemParams *params = itemsParams.value(itemToMountId).get();

	if (((!(params == nullptr)) && (!(this->garageWindow == nullptr))))
	{
		//this->garageWindow->mountItem(itemToMountId);
		//this->mountedItems->push(itemToMountId);

		if (params->itemType == ItemTypeEnum::WEAPON || params->itemType == ItemTypeEnum::ARMOR || params->itemType == ItemTypeEnum::COLOR)
		{
			switch (params->itemType)
			{
				case ItemTypeEnum::WEAPON:
					this->mountedWeaponId = itemToMountId;
					mounted[0] = this->mountedWeaponId;
					this->mountedWeaponInfo = itemsInfo[itemToMountId];
					this->mountedWeaponParams = params;
					this->setTurret(itemToMountId);
					break;
				case ItemTypeEnum::ARMOR:
					this->mountedArmorId = itemToMountId;
					mounted[1] = this->mountedArmorId;
					this->mountedArmorInfo = itemsInfo[itemToMountId];
					this->mountedArmorParams = itemsParams[itemToMountId].get();
					this->setHull(itemToMountId);
					break;
				case ItemTypeEnum::COLOR:
					this->mountedEngineId = itemToMountId;
					mounted[2] = this->mountedEngineId;
					this->mountedEngineInfo = itemsInfo[itemToMountId];
					this->mountedEngineParams = itemsParams[itemToMountId].get();
#ifdef GRAPHICS
					this->setColorMap(ResourceUtil::getResourceImage(itemToMountId));
#endif
					break;
				default:
					break;
			}
		}
	}
	else
	{
		//this->itemsForMount.push(itemToMountId);
	}

	//if (((this->garageWindow->selectedItemId == itemToMountId) || (this->garageWindow->selectedItemId == itemToMountId.replace("HD_", ""))))
	//{
		//this->garageWindow->selectItemInWarehouse(itemToMountId.replace("HD_", ""));
		//this->garageWindow->lockMountButton();
	//}

	this->lockMount = false;
}

void GarageModel::setHull(const QString &resource)
{
	this->garageWindow->tankPreview->setHull(resource);
}

void GarageModel::setTurret(const QString &resource)
{
	this->garageWindow->tankPreview->setTurret(resource);
}

void GarageModel::setColorMap(ImageResource *map)
{
	this->garageWindow->tankPreview->setColorMap(map);
}
