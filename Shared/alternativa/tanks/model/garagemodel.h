#ifndef GARAGEMODEL_H
#define GARAGEMODEL_H


#include <QString>
#include <QHash>
#include <memory>
#include "alternativa/tanks/model/itemparams.h"
#include "com/alternativaplatform/projects/tanks/client/garage/garage/iteminfo.h"


class GarageWindow;
class Network;
class Long;
class ClientObject;
class PartsList;
class ItemInfo;
class ImageResource;
class QObject;


class GarageModel
{
	/*
	private var dialogsLayer:DisplayObjectContainer;
	private var helpService:IHelpService;
	private var modelRegister:IModelService;
	private var resourceRegister:IResourceService;
	private var panelModel:IPanel;
	private var itemModel:IItem;
	private var layer:DisplayObjectContainer;
	private var itemsForMount:Array;
	private var storeSelectedItem:String;
	private var warehouseSelectedItem:String;
	private var firstItemId:String;
	private var minItemIndex:int = 2147483647;
	private var firstItemParams:ItemParams;
	private var lockBuy:Boolean = false;
	private var lockSell:Boolean = false;
	private var lockMount:Boolean = false;
	private var lockReplace:Boolean = false;
	private var lockUpgrade:Boolean = false;
	private var itemDumper:ItemDumper;
	private var storeHelper:StoreListHelper;
	private var warehouseHelper:WarehouseListHelper;
	private var confirmAlert:ConfirmAlert;
	private var itemWaitingForConfirmation:String;

	private var localeService:ILocaleService;
	private var items:Dictionary = new Dictionary();
	private var i:int = 2;

*/

	bool lockMount;


	Long *garageBoxId;
	ClientObject *clientObject;
	Network *socket;

	QHash<QString, std::shared_ptr<ItemInfo>> itemsInfo;
	QHash<QString, std::shared_ptr<ItemParams>> items;

	PartsList *warehouseList;

	QObject *o;


public:

	QHash<QString, ItemInfo> info;
	static QHash<QString, std::shared_ptr<ItemParams>> itemsParams;

	//itemsParams:Dictionary;
	//itemsInfo:Dictionary;
	QString mounted[3];
	//buyCrystalCurrency:String;
	//buyCrystalRate:Number;


	//private const HELPER_STORE:int = 2;
	//private const HELPER_WAREHOUSE:int = 3;
	//private const HELPER_GROUP_KEY:String = "GarageModel";

	QString mountedWeaponId;
	QString mountedArmorId;
	QString mountedEngineId;


	std::shared_ptr<ItemInfo> mountedWeaponInfo;
	std::shared_ptr<ItemInfo> mountedArmorInfo;
	std::shared_ptr<ItemInfo> mountedEngineInfo;


	ItemParams *mountedWeaponParams;
	ItemParams *mountedArmorParams;
	ItemParams *mountedEngineParams;


	/*
	public var mountedItems:Array;
	public var mountedWeaponId:String;
	public var mountedWeaponInfo:ItemInfo;
	public var mountedWeaponParams:ItemParams;
	public var mountedArmorId:String;
	public var mountedArmorInfo:ItemInfo;
	public var mountedArmorParams:ItemParams;
	public var mountedEngineId:String;
	public var mountedEngineInfo:ItemInfo;
	public var mountedEngineParams:ItemParams;
	public var currentItemForUpdate:String;
	public var kostil:Boolean = false;
	*/

	bool kostil;

	GarageWindow *garageWindow;

	GarageModel();




	void initObject(ClientObject *clientObject, const QString &country, double rate, const Long &garageBoxId, Network *networker);

	void objectLoaded(ClientObject *object);

	void objectUnloaded(ClientObject *object);

	void initItem(const QString &id, std::shared_ptr<ItemParams> item);

	void initDepot(ClientObject *clientObject, QVector<std::shared_ptr<ItemInfo>> &itemsOnSklad);

	static ItemParams* getItemParams(const QString &itemId);

	void mountItem(ClientObject *clientObject, const QString &itemToUnmountId, const QString &itemToMountId);

	void setHull(const QString &resource);
	void setTurret(const QString &resource);
	void setColorMap(ImageResource *map);
};


#endif // GARAGEMODEL_H
