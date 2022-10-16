#include "imageresource.h"
#include "objectbitmapdata.h"
#include "alternativa/resource/stubbitmapdata.h"
#include "file.h"
#include "imageresource.h"
#include "service.h"
#include <QDebug>


ImageResource::ImageResource(std::shared_ptr<ObjectBitmapData> &bitmapData, const QString &id, bool animatedMaterial,
							 MultiframeResourceData *multiframeData, const QString &url)
{
	this->isWeb = false;
	this->isSVG = false;

	this->bitmapData = bitmapData;
	this->id = id;
	this->animatedMaterial = animatedMaterial;
	this->multiframeData = multiframeData;
	this->url = url;

	//qDebug() << "ImageResource" << id << animatedMaterial << multiframeData << url;


	if (bitmapData->data == nullptr)
	{
		QString url = bitmapData->url.toLower();

		if (url.contains(".webp")) {
			this->type = ImageType::WEBP;
		} else if (url.contains(".svg")) {
			this->type = ImageType::SVG;
		} else if (url.contains(".jp2")) {
			this->type = ImageType::JPEG2000;
		} else {
			this->type = ImageType::COMMON;
		}
	}
	else
	{
		this->type = ImageType::COMMON;
	}
}


void ImageResource::load()
{
	if (this->loaded())
		return;

	QString url = this->url;

	if (url.indexOf(QChar('?')) >= 0)
		url = url.split(QChar('?'))[0];

	QString filename = Service::getResourcesPath() + url;

	QByteArray data;
	if (File::read(filename, data))
	{
		this->onPictureLoadingComplete(data);
	}
	else
	{
		qDebug("ImageResource::load() error");
		qDebug() << filename;
		this->onPictureLoadingError();
	}




	/*
	if (this->type == ImageType::WEBP)
	{
		this->imageLoader = new URLLoader();
		this->imageLoader.dataFormat = URLLoaderDataFormat.BINARY;
		this->imageLoader.addEventListener(Event.COMPLETE, this->onPictureLoadingComplete);
		this->imageLoader.addEventListener(IOErrorEvent.IO_ERROR, this->onPictureLoadingError);
		this->imageLoader.load(new URLRequest(this->url));
	}
	else
	{
		if (this->type == ImageType::COMMON)
		{
			this->imageLoader = new CacheURLLoader();
			this->imageLoader.dataFormat = URLLoaderDataFormat.BINARY;
			this->imageLoader.addEventListener(Event.COMPLETE, this->onPictureLoadingComplete);
			this->imageLoader.addEventListener(IOErrorEvent.IO_ERROR, this->onPictureLoadingError);
			this->imageLoader.load(new URLRequest(this->url));
		}
	}
	*/

	//this->loading = true;
}

bool ImageResource::loaded()
{
	return this->bitmapData != nullptr && this->bitmapData->data != nullptr;
}

void ImageResource::onPictureLoadingComplete(const QByteArray &data)
{
	if (this->type == ImageType::WEBP)
	{
		qDebug("used ImageResource::onPictureLoadingComplete WEBP");
		//this->bitmapData = WebpSwc.decode(ByteArray(e.currentTarget.data));
		//this->destroyImageLoader();
		//if (this->completeLoadListener != nullptr)
		//this->completeLoadListener.resourceLoaded(this);
	}
	else
	{
		auto bitmap = std::make_shared<BitmapData>();
		if (!bitmap->loadFromData(data))
		{
			//static int c;
			//File::save("image error " + QString::number(++c) + ".png", data);

			qDebug("error ImageResource::onNativeLoaderComplete");
			return;
		}



		this->bitmapData = std::make_shared<ObjectBitmapData>();
		this->bitmapData->data = bitmap;

		//this->nativeLoader = new Loader();
		//this->nativeLoader.contentLoaderInfo.addEventListener(Event.COMPLETE, this.onNativeLoaderComplete);
		//this->nativeLoader.loadBytes(e.target.data);
	}
}

void ImageResource::onPictureLoadingError()
{
	this->bitmapData = std::make_shared<ObjectBitmapData>();
	this->bitmapData->data = std::make_shared<StubBitmapData>(0xFF0000);
	//this->destroyImageLoader();
	//if (this->completeLoadListener != nullptr)
	//this->completeLoadListener.resourceLoaded(this);
}
