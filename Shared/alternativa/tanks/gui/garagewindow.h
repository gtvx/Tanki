#ifndef GARAGEWINDOW_H
#define GARAGEWINDOW_H

#include <memory>
#include <QStringList>

class Long;
class TankPreview;
class ItemParams;
class ItemInfo;
class QString;
class PartsList;


class GarageWindow
{
	QStringList itemsInWarehouse;
	//PartsList *warehouseList;
public:
	//private const windowMargin:int = 11;
	//private const buttonSize:Point = new Point(104, 33);


	//private var resourceRegister:IResourceService;
	//private var modelRegister:IModelService;
	//private var localeService:ILocaleService;
	//private var windowSize:Point;
	//private var myItemsWindow:TankWindow;
	//private var myItemsInner:TankWindowInner;
	//private var warehouseList:PartsList;
	//private var shopItemsWindow:TankWindow;
	//private var shopItemsInner:TankWindowInner;
	//private var storeList:PartsList;

	//private var itemsInWarehouse:Array;
	//private var itemsInStore:Array;

	int i;
	int j;


	const int itemInfoPanelWidth = 412;
	//ItemInfoPanel *itemInfoPanel;
	bool inventorySelected;
	bool storeItemSelected;
	//public var selectedItemId:String;
	TankPreview *tankPreview;


	GarageWindow(Long *garageBoxId);

	void addItemToWarehouse(const QString &itemId, std::shared_ptr<ItemParams> itemParams, std::shared_ptr<ItemInfo> itemInfo);

};

#endif // GARAGEWINDOW_H
