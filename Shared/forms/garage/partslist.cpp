#include "partslist.h"
#include <QVariant>


//private static const MIN_POSIBLE_SPEED:Number = 70;
//private static const MAX_DELTA_FOR_SELECT:Number = 7;
//private static const ADDITIONAL_SCROLL_AREA_HEIGHT:Number = 3;
//private static var _discountImage:Class = PartsList__discountImage;
//private static var discountBitmap:BitmapData = new _discountImage().bitmapData;

PartsList::PartsList(const QString &id)
{
	scrollSpeed = 0;
	this->ID = id;
	//this->dp = new DataProvider();
	//this->silentdp = new DataProvider();
	//this->silentList = new TileList();
	//this->silentList.dataProvider = this->silentdp;
	//this->list = new TileList();
	//this->list->dataProvider = this->dp;
	//this->list->addEventListener(ListEvent.ITEM_CLICK, this->selectItem);
	//this->list->addEventListener(ListEvent.ITEM_DOUBLE_CLICK, this->selectItem);
	//this->list->rowCount = 1;
	//this->list->rowHeight = 130;
	//this->list->columnWidth = 203;
	//this->list->setStyle("cellRenderer", PartsListRenderer);
	//this->list->direction = ScrollBarDirection.HORIZONTAL;
	//this->list->focusEnabled = false;
	//this->list->horizontalScrollBar.focusEnabled = false;
	//addChild(this->list);
	//addEventListener(MouseEvent.MOUSE_WHEEL, this->scrollList);
	//this->list.horizontalScrollBar.addEventListener(Event.ENTER_FRAME, this->updateScrollOnEnterFrame);
	//addEventListener(MouseEvent.MOUSE_DOWN, this->onMouseDown);
	//this->confScroll();
	//addEventListener(Event.REMOVED_FROM_STAGE, this->killLists);
}

/*
void PartsList::mount(Object *id)
{
	this->update(id, "installed", true);
}
*/

void PartsList::update(const QString &id, const QString &param, bool value)
{
	(void)id;
	(void)param;
	(void)value;

	/*
	bool HD = (!(String(id).indexOf("HD_") == -1));
	bool M3 = ((!(id.indexOf("_m3") == -1)) && ((!(id.indexOf("dictator") == -1)) || (!(id.indexOf("railgun") == -1))));

	if (HD == true)
	{
		id = id.replace("HD_", "");
	}

	Object *obj;

	int index = this.indexById(id);
	if (M3 && HD)
	{
		if (index != -1)
		{
			obj = this->silentdp->getItemAt(this.indexById(("HD_" + id), true));
			this->silentdp->addItem(this->dp->getItemAt(index));
		}
		else
		{
			obj = this->silentdp->getItemAt(this->indexById(id, true));
			index = this->indexById("HD_" + id);
		}
	}
	else
	{
		if (index != -1)
		{
			obj = this->dp->getItemAt(index);
		}
		else
		{
			index = this->indexById(("HD_" + id));
			obj = this->dp->getItemAt(index);
		}
	}
	*/

	//QHash<QString, QVariant> data = obj->dat;

	//Object *data = obj->dat;
	//data[param].setValue(value);
	//DisplayObject *iNormal = this->myIcon(data, false);
	//DisplayObject *iSelected = this->myIcon(data, true);
	//obj->dat = data;
	//obj->iconNormal = iNormal;
	//obj->iconSelected = iSelected;
	//this->dp->replaceItemAt(obj, index);
	//this->dp->sortOn(["accessable", "rang", "typesort", "sort"], [Array.DESCENDING, Array.NUMERIC, Array.NUMERIC, Array.NUMERIC]);
	//this->dp->invalidateItemAt(index);
}
