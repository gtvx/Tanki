#include "imageresourceloader.h"
#include "imageresourcelist.h"
#include "imageresource.h"
#include "MultiframeResourceData.h"
#include "objectbitmapdata.h"
#include "flash/display/bitmapdata.h"
#include "file.h"
#include "service.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


ImageResourceLoader::ImageResourceLoader(const QString &path)
{
	length = 0;
	lengthFailed = 0;
	status = 0;

	this->path = path;
	this->list = new ImageResourceList();
	this->inbattleList = new ImageResourceList();
	//this->queue = new QVector<ImageResource>();
	//this->config = new Dictionary();
	//var loader:URLLoader = new URLLoader();
	//loader.addEventListener(Event.COMPLETE, this->parse);
	//loader.load(new URLRequest(path));

	this->load();
}

ImageResourceLoader::~ImageResourceLoader()
{
	this->clear();

	delete this->list;
	delete this->inbattleList;
}

void ImageResourceLoader::clear()
{
	this->list->clear();
	this->inbattleList->clear();
}

/*
void ImageResourceLoader::reload()
{
	this->list->clear();
	delete this->list;
	this->list = new ImageResourceList();
	this->list->images = this->inbattleList->images;
	this->inbattleList = new ImageResourceList();
	//this->queue = new Vector.<ImageResource>();
	//var loader:URLLoader = new URLLoader();
	//loader.addEventListener(Event.COMPLETE, this->parse);
	//loader.load(new URLRequest(this->path));

	this->load();
}
*/

void ImageResourceLoader::load()
{
	QString filename = Service::getClientPath() + this->path;
	QByteArray data;

	if (File::read(filename, data))
	{
		this->parse(data);
	}
	else
	{
		qDebug("error ImageResourceLoader::load()");
	}
}

void ImageResourceLoader::parse(const QByteArray &data)
{
	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (doc.isNull()) {
		qDebug("error ImageResourceLoader::parse()");
		return;
	}

	QJsonObject config = doc.object();


	if (config.value("id").toString() == "IMAGES")
	{
		QJsonArray items = config.value("items").toArray();

		for (int i = 0; i < items.count(); i++)
		{
			QJsonObject obj = items.at(i).toObject();
			QString name = obj.value("name").toString();


			if (!this->list->getImage(name))
			{
				QString src = obj.value("src").toString();

				MultiframeResourceData *multiframeInfo = nullptr;

				bool multiframed = obj.value("multiframe").toBool(false);

				if (multiframed)
				{
					multiframeInfo = new MultiframeResourceData();
					multiframeInfo->fps = obj.value("fps").toInt();
					multiframeInfo->heigthFrame = obj.value("frame_heigth").toInt();
					multiframeInfo->widthFrame = obj.value("frame_width").toInt();
					multiframeInfo->numFrames = obj.value("num_frames").toInt();
				}

				QString path = src;
				/*
				if (((IStorageService(Main.osgi.getService(IStorageService)).getStorage().data["use_old_textures"] == true) && ((!(path.search("hull/") == -1)) || (!(path.search("turret/") == -1)))))
				{
					QString oldPath = path.split("/")[1];
					QString newPath = (oldPath + "_old");
					path = path.replace(oldPath, newPath);
				}
				*/

				auto bitmapData = std::make_shared<ObjectBitmapData>();
				bitmapData->url = path;

				ImageResource *res = new ImageResource(bitmapData, name, multiframed, multiframeInfo, path);
				this->queue.append(res);
				this->config[name] = res;
			}
		}

		this->loadQueue();
	}
}

void ImageResourceLoader::loadQueue()
{
	for (ImageResource *obj : this->queue)
	{
		this->loadImage(obj);
		this->length++;
	}
}

void ImageResourceLoader::loadImage(ImageResource *img)
{
	//var loader:Object;
	//var svg:SVGImage;
	//var f:Function;
	//var url:String;


	//if (img->id.indexOf("preview") >= 0)
	{
		//img->url = ((prefix + img.bitmapData) as String);
		img->url = img->bitmapData->url;

		this->list->add(img);

		if (this->length == 1 && this->lengthFailed <= 0)
		{
			this->status = 1;
			//ResourceUtil.onCompleteLoading();
			//this->failedResources = new Dictionary();
			qDebug() << "failedResources" << img->url;
		}

		this->length--;
		return;
	}



	switch (img->type)
	{
		case ImageType::SVG:
			/*
			svg = new SVGImage();
			f = svg.addListener(function ():void
								{
									img.bitmapData = new BitmapData(svg.width, svg.height, false, 0);
									BitmapData(img.bitmapData).draw(svg, svg.transform.matrix);
									svg.removeListener(f);
								});
			svg.loadURL(((prefix + img.bitmapData) as String));
			*/
			break;

		case ImageType::WEBP:
			/*
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, function (e:Event):void
									{
										var bitmapData:BitmapData = WebpSwc.decode(ByteArray(e.currentTarget.data));
										if ((!(bitmapData)))
										{
											bitmapData = new BitmapData(100, 100, true, 0xFFFFFF);
										}
										var url:String = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = url.split("?")[0];
										}
										list.add(new ImageResource(bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
										var failedImage:FailedResource = failedResources[url];
										if (failedImage != null)
										{
											lengthFailed--;
										}
										if (((length == 1) && (lengthFailed <= 0)))
										{
											status = 1;
											ResourceUtil.onCompleteLoading();
											failedResources = new Dictionary();
										}
										length--;
									});
			loader.addEventListener(IOErrorEvent.IO_ERROR, function (e:IOErrorEvent):void
									{
										trace((("Can't load resource: " + prefix) + img.bitmapData));
										Main.debug.showAlert((("Can't load resource: " + prefix) + img.bitmapData));
										var url:String = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = url.split("?")[0];
										}
										var failedImage:FailedResource = failedResources[url];
										if (failedImage == null)
										{
											failedImage = new FailedResource();
											failedResources[url] = failedImage;
											lengthFailed++;
										}
										failedImage.failedCount++;
										url = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = ((url.split("?")[0] + "?rand=") + Math.random());
										}
										else
										{
											url = (url + ("?rand=" + Math.random()));
										}
										img.bitmapData = url;
										if (failedImage.failedCount >= 3)
										{
											trace(((("Can't load resource: " + prefix) + img.bitmapData) + " after 3 attempts."));
											Main.debug.showAlert(((("Can't load resource: " + prefix) + img.bitmapData) + " after 3 attempts."));
											img.bitmapData = new StubBitmapData(0xFF0000);
											lengthFailed--;
											list.add(new ImageResource(img.bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
											if (((length == 1) && (lengthFailed <= 0)))
											{
												status = 1;
												ResourceUtil.onCompleteLoading();
												failedResources = new Dictionary();
											}
											length--;
										}
										else
										{
											loadImage(img);
										}
									});
			loader.load(new URLRequest(((prefix + img.bitmapData) as String)));
			*/
			break;

		case ImageType::COMMON:

		{
			//nativeLoader.loadBytes(event.target.data);
			QString url = img->bitmapData->url;
			if (url.indexOf(QChar('?')) >= 0)
				url = url.split(QChar('?'))[0];


			QString filename = Service::getResourcesPath() + url;

			QByteArray data;
			if (File::read(filename, data))
			{
				auto bitmapData = std::make_shared<ObjectBitmapData>();
				bitmapData->data = std::make_shared<BitmapData>();

				if (!bitmapData->data->loadFromData(data))
				{
					qDebug() << "error bitmapData->loadFromData" << filename;
					return;
				}

				QString prefix = "";

				list->add(new ImageResource(bitmapData, img->id, img->animatedMaterial, img->multiframeData, prefix + url));

				//FailedResource *failedImage = failedResources[url];
				//if (failedImage != nullptr)
				//lengthFailed--;

				if (length == 1 && lengthFailed <= 0)
				{
					status = 1;
					//ResourceUtil::onCompleteLoading();
					//failedResources = new Dictionary();
				}

				length--;
			}
			else
			{
				qDebug("error ImageResourceLoader::loadImage()");
				qDebug() << filename;
			}

		}
			/*
			loader = new CacheURLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, function (event:Event):void
									{
										var nativeLoader:Loader = new Loader();
										nativeLoader.contentLoaderInfo.addEventListener(Event.COMPLETE, function (e:Event):void
										{
											var bitmapData:BitmapData = Bitmap(e.target.content).bitmapData;
											var url:String = (img.bitmapData as String);
											if (url.indexOf("?") >= 0)
											{
												url = url.split("?")[0];
											}
											list.add(new ImageResource(bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
											var failedImage:FailedResource = failedResources[url];
											if (failedImage != null)
											{
												lengthFailed--;
											}
											if (((length == 1) && (lengthFailed <= 0)))
											{
												status = 1;
												ResourceUtil.onCompleteLoading();
												failedResources = new Dictionary();
											}
											length--;
										});
										try
										{
											nativeLoader.loadBytes(event.target.data);
										}
										catch(e:Error)
										{
											(Main.osgi.getService(IConsole) as IConsole).addLine((((img.id + " ") + img.url) + e.getStackTrace()));
										}
									});
			loader.addEventListener(IOErrorEvent.IO_ERROR, function (e:IOErrorEvent):void
									{
										trace((("Can't load resource: " + prefix) + img.bitmapData));
										(Main.osgi.getService(IConsole) as IConsole).addLine((("resource: error " + prefix) + img.bitmapData));
										Main.debug.showAlert((("Can't load resource: " + prefix) + img.bitmapData));
										var url:String = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = url.split("?")[0];
										}
										var failedImage:FailedResource = failedResources[url];
										if (failedImage == null)
										{
											failedImage = new FailedResource();
											failedResources[url] = failedImage;
											lengthFailed++;
										}
										failedImage.failedCount++;
										url = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = ((url.split("?")[0] + "?rand=") + Math.random());
										}
										else
										{
											url = (url + ("?rand=" + Math.random()));
										}
										img.bitmapData = url;
										if (failedImage.failedCount >= 3)
										{
											trace(((("Can't load resource: " + prefix) + img.bitmapData) + " after 3 attempts."));
											Main.debug.showAlert(((("Can't load resource: " + prefix) + img.bitmapData) + " after 3 attempts."));
											img.bitmapData = new StubBitmapData(0xFF0000);
											lengthFailed--;
											list.add(new ImageResource(img.bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
											if (((length == 1) && (lengthFailed <= 0)))
											{
												status = 1;
												ResourceUtil.onCompleteLoading();
												failedResources = new Dictionary();
											}
											length--;
										}
										else
										{
											loadImage(img);
										}
									});
			url = (img.bitmapData as String);
			if (url.indexOf("?") >= 0)
			{
				url = url.split("?")[0];
			}
			loader.load(new URLRequest((prefix + url)));
			*/
			break;

		case ImageType::JPEG2000:
			break;
	}
}

void ImageResourceLoader::loadForBattle(const QString &id)
{
	if ((!(this->inbattleList->isLoaded(id))))
	{
		//ResourceUtil::loading.push(id);
		this->loadImageSingle(this->config[id]);
	}
}

void ImageResourceLoader::loadImageSingle(ImageResource *img)
{

	//var loader:Object;
	//var svg:SVGImage;
	//var f:Function;

	//QString prefix = ((Game.local) ? "" : "resources/");

	switch (img->type)
	{
		case ImageType::SVG:
			/*
			svg = new SVGImage();
			f = svg.addListener(function ():void
								{
									img.bitmapData = new BitmapData(svg.width, svg.height, false, 0);
									BitmapData(img.bitmapData).draw(svg, svg.transform.matrix);
									svg.removeListener(f);
								});
			svg.loadURL(((prefix + img.bitmapData) as String));
			*/
			break;

		case ImageType::WEBP:
			/*
			loader = new URLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			loader.addEventListener(Event.COMPLETE, function (e:Event):void
									{
										var bitmapData:BitmapData = WebpSwc.decode(ByteArray(e.currentTarget.data));
										if ((!(bitmapData)))
										{
											bitmapData = new BitmapData(100, 100, true, 0xFFFFFF);
										}
										var url:String = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = url.split("?")[0];
										}
										inbattleList.add(new ImageResource(bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
										var failedImage:FailedResource = failedResources[url];
										if (failedImage != null)
										{
											lengthFailed--;
										}
										ResourceUtil.loading.removeAt(ResourceUtil.loading.indexOf(img.id));
										if (ResourceUtil.loading.length == 0)
										{
											ResourceUtil.onCompleteLoading();
										}
										if (((length == 1) && (lengthFailed <= 0)))
										{
											length--;
										}
									});
			loader.addEventListener(IOErrorEvent.IO_ERROR, function (e:IOErrorEvent):void
									{
										Main.debug.showAlert((("Can't load resource: " + prefix) + img.bitmapData));
										var url:String = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = url.split("?")[0];
										}
										var failedImage:FailedResource = failedResources[url];
										if (failedImage == null)
										{
											failedImage = new FailedResource();
											failedResources[url] = failedImage;
											lengthFailed++;
										}
										failedImage.failedCount++;
										url = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = ((url.split("?")[0] + "?rand=") + Math.random());
										}
										else
										{
											url = (url + ("?rand=" + Math.random()));
										}
										img.bitmapData = url;
										if (failedImage.failedCount >= 3)
										{
											Main.debug.showAlert(((("Can't load resource: " + prefix) + img.bitmapData) + " after 3 attempts."));
											img.bitmapData = new StubBitmapData(0xFF0000);
											lengthFailed--;
											inbattleList.add(new ImageResource(img.bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
											ResourceUtil.loading.removeAt(ResourceUtil.loading.indexOf(img.id));
											if (ResourceUtil.loading.length == 0)
											{
												ResourceUtil.onCompleteLoading();
											}
											if (((length == 1) && (lengthFailed <= 0)))
											{
												status = 1;
												failedResources = new Dictionary();
											}
											length--;
										}
										else
										{
											loadImage(img);
										}
									});
			loader.load(new URLRequest(((prefix + img.bitmapData) as String)));
			*/
			break;

		case ImageType::COMMON:
		{
			//var bitmapData:BitmapData = Bitmap(e.target.content).bitmapData;
			//(Main.osgi.getService(IConsole) as IConsole).addLine((("resource: " + img.id) + "loaded"));

			QString url = img->bitmapData->url;
			if (url.indexOf(QChar('?')) >= 0)
				url = url.split(QChar('?'))[0];

			QByteArray data;
			if (File::read(url, data))
			{
				auto bitmapData = std::make_shared<ObjectBitmapData>();
				bitmapData->data = std::make_shared<BitmapData>();


				QString prefix = "";

				inbattleList->add(new ImageResource(bitmapData, img->id, img->animatedMaterial, img->multiframeData, prefix + url));

				//FailedResource *failedImage = failedResources[url];
				//if (failedImage != nullptr)
				//lengthFailed--;

				//ResourceUtil.loading.removeAt(ResourceUtil.loading.indexOf(img.id));
				//if (ResourceUtil.loading.length == 0)
					//ResourceUtil.onCompleteLoading();

				if (length == 1 && lengthFailed <= 0)
					length--;
			}
			else
			{
				qDebug("error ImageResourceLoader::loadImageSingle()");
			}
		}


			/*
			loader = new CacheURLLoader();
			loader.dataFormat = URLLoaderDataFormat.BINARY;
			(Main.osgi.getService(IConsole) as IConsole).addLine(("start loading resource: " + img.id));
			loader.addEventListener(Event.COMPLETE, function (event:Event):void
									{
										var nativeLoader:Loader = new Loader();
										nativeLoader.contentLoaderInfo.addEventListener(Event.COMPLETE, function (e:Event):void
										{
											var bitmapData:BitmapData = Bitmap(e.target.content).bitmapData;
											(Main.osgi.getService(IConsole) as IConsole).addLine((("resource: " + img.id) + "loaded"));
											var url:String = (img.bitmapData as String);
											if (url.indexOf("?") >= 0)
											{
												url = url.split("?")[0];
											}
											inbattleList.add(new ImageResource(bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
											var failedImage:FailedResource = failedResources[url];
											if (failedImage != null)
											{
												lengthFailed--;
											}
											ResourceUtil.loading.removeAt(ResourceUtil.loading.indexOf(img.id));
											if (ResourceUtil.loading.length == 0)
											{
												ResourceUtil.onCompleteLoading();
											}
											if (((length == 1) && (lengthFailed <= 0)))
											{
												length--;
											}
										});
										nativeLoader.loadBytes(event.target.data);
									});
			loader.addEventListener(IOErrorEvent.IO_ERROR, function (e:IOErrorEvent):void
									{
										Main.debug.showAlert((("Can't load resource: " + prefix) + img.bitmapData));
										var url:String = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = url.split("?")[0];
										}
										var failedImage:FailedResource = failedResources[url];
										if (failedImage == null)
										{
											failedImage = new FailedResource();
											failedResources[url] = failedImage;
											lengthFailed++;
										}
										failedImage.failedCount++;
										url = (img.bitmapData as String);
										if (url.indexOf("?") >= 0)
										{
											url = ((url.split("?")[0] + "?rand=") + Math.random());
										}
										else
										{
											url = (url + ("?rand=" + Math.random()));
										}
										img.bitmapData = url;
										if (failedImage.failedCount >= 3)
										{
											Main.debug.showAlert(((("Can't load resource: " + prefix) + img.bitmapData) + " after 3 attempts."));
											img.bitmapData = new StubBitmapData(0xFF0000);
											lengthFailed--;
											inbattleList.add(new ImageResource(img.bitmapData, img.id, img.animatedMaterial, img.multiframeData, (prefix + url)));
											ResourceUtil.loading.removeAt(ResourceUtil.loading.indexOf(img.id));
											if (ResourceUtil.loading.length == 0)
											{
												ResourceUtil.onCompleteLoading();
											}
											if (((length == 1) && (lengthFailed <= 0)))
											{
												status = 1;
												failedResources = new Dictionary();
											}
											length--;
										}
										else
										{
											loadImage(img);
										}
									});
			loader.load(new URLRequest(((prefix + img.bitmapData) as String)));
			*/
			break;

		case ImageType::JPEG2000:
			break;
	}
}
