#include "imageresourcelist.h"
#include "imageresource.h"
#include <QHashIterator>
#include "objectbitmapdata.h"


ImageResourceList::ImageResourceList()
{

}

void ImageResourceList::add(ImageResource *img)
{
	if (!this->images.contains(img->id))
	{
		if (img->bitmapData != nullptr)
		{
			this->images.insert(img->id, img);
			//this->ids.append(img->id);
		}
		else
		{
			throw 4132413;
			//throw (new Error(("Bitmap null! " + img.id)));
		}
	}
	else
	{
		img->bitmapData = nullptr;
	}
}

ImageResource* ImageResourceList::getImage(const QString &key)
{
	return this->images.value(key);
}

bool ImageResourceList::isLoaded(const QString &key)
{
	auto image = this->images.value(key);
	return image != nullptr && image->bitmapData != nullptr;
	//return !((this->images.value(key) == nullptr) || ((this->images[key].bitmapData as BitmapData) == nullptr));
}

void ImageResourceList::clear()
{
	QHashIterator<QString, ImageResource*> i(this->images);
	while (i.hasNext())
	{
		i.next();
		ImageResource *image = i.value();
		//delete image;
		image->bitmapData = nullptr;
	}

	//this->images.clear();
	//this->ids.clear();
}
