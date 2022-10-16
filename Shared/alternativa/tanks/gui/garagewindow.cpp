#include "garagewindow.h"
#include "alternativa/tanks/model/tankpreview.h"
#include "resource/resourceutil.h"


GarageWindow::GarageWindow(Long *garageBoxId)
{
	i = 0;
	j = 0;
	inventorySelected = false;
	storeItemSelected = false;


	//this->resourceRegister = (Main.osgi.getService(IResourceService) as IResourceService);
	//this->modelRegister = (Main.osgi.getService(IModelService) as IModelService);
	//this->localeService = (Main.osgi.getService(ILocaleService) as ILocaleService);
	//this->itemsInWarehouse = new Array();
	//this->itemsInStore = new Array();
	//this->windowSize = new Point(880, 737);
	this->tankPreview = new TankPreview(garageBoxId);
	//addChild(this->tankPreview);
	//this->itemInfoPanel = new ItemInfoPanel();
	//addChild(this->itemInfoPanel);

	//if (TankWindow(Main.osgi.getService(TankWindow)) != null)
	//this->myItemsWindow = TankWindow(Main.osgi.getService(TankWindow));
	//else
	//this->myItemsWindow = new TankWindow();
	//Main.osgi.registerService(TankWindow, this->myItemsWindow);

	//addChild(this->myItemsWindow);
	//this->myItemsWindow.headerLang = this->localeService.getText(TextConst.GUI_LANG);
	//this->myItemsWindow.header = TankWindowHeader.YOUR_ITEMS;
	//this->myItemsInner = new TankWindowInner(0, 0, TankWindowInner.GREEN);
	//this->myItemsInner.showBlink = true;
	//addChild(this->myItemsInner);
	//this->warehouseList = new PartsList("warehouse");
	//addChild(this->warehouseList);
	//this->shopItemsWindow = new TankWindow();
	//addChild(this->shopItemsWindow);
	//this->shopItemsWindow.headerLang = this->localeService.getText(TextConst.GUI_LANG);
	//this->shopItemsWindow.header = TankWindowHeader.SHOP;
	//this->shopItemsInner = new TankWindowInner(0, 0, TankWindowInner.GREEN);
	//this->shopItemsInner.showBlink = true;
	//addChild(this->shopItemsInner);
	//this->storeList = new PartsList("market");
	//addChild(this->storeList);
	//this->warehouseList.addEventListener(PartsListEvent.SELECT_PARTS_LIST_ITEM, this->onWarehouseItemSelect);
	//this->storeList.addEventListener(PartsListEvent.SELECT_PARTS_LIST_ITEM, this->onStoreItemSelect);
	//this->itemInfoPanel.buttonBuy.addEventListener(MouseEvent.CLICK, this->onButtonBuyClick);
	//this->itemInfoPanel.buttonEquip.addEventListener(MouseEvent.CLICK, this->onButtonEquipClick);
	//this->itemInfoPanel.buttonSkin.addEventListener(MouseEvent.CLICK, this->onSkinButtonClick);
	//this->itemInfoPanel.buttonUpgrade.addEventListener(MouseEvent.CLICK, this->onModButtonClick);
	//this->itemInfoPanel.buttonBuyCrystals.addEventListener(MouseEvent.CLICK, this->onButtonBuyCrystalsClick);
}

void GarageWindow::addItemToWarehouse(const QString &itemId, std::shared_ptr<ItemParams> itemParams, std::shared_ptr<ItemInfo> itemInfo)
{
	(void)itemParams;
	(void)itemInfo;
	this->itemsInWarehouse.append(itemId);
	//ImageResource *resource = ResourceUtil::getResourceImage(itemId + "_preview");
	//this->warehouseList->addItem(itemId, itemParams.name, itemParams.itemType.value, itemParams.itemIndex, itemParams.price, itemInfo.discount, itemParams.rankId, false, true, itemInfo.count, resource, itemParams.modificationIndex);
}
