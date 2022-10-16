#include "controlpointsprite.h"
#include "resource/resourceutil.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "flash/geom/matrix.h"
#include "alternativa/gfx/core/bitmaptextureresource.h"
#include "flash/display3D/textures/texture.h"
#include <QPainter>


const Object3D::T *ControlPointSprite::TYPE = new Object3D::T("ControlPointSprite");

static const int BALANCE_ABS = 100;
static const int BALANCE_NONE = 0;
static const int BALANCE_RED = -100;
static const int BALANCE_BLUE = 100;

static std::shared_ptr<BitmapData> red;
static std::shared_ptr<BitmapData> blue;
static std::shared_ptr<BitmapData> none;

static std::shared_ptr<BitmapData> abds;
static std::shared_ptr<BitmapData> blues;
static std::shared_ptr<BitmapData> reds;
static std::shared_ptr<BitmapData> nones;
static std::shared_ptr<BitmapData> chars;

static std::shared_ptr<BitmapData> get_abds()
{
	if (abds == nullptr)
		abds = ResourceUtil::getResourceImageLocal("dom/ControlPointSprite_alphaC.png");
	return abds;
}

static std::shared_ptr<BitmapData> get_blues()
{
	if (blues == nullptr)
		blues = ResourceUtil::getResourceImageLocal("dom/ControlPointSprite_blueC.png");
	return blues;
}

static std::shared_ptr<BitmapData> get_reds()
{
	if (reds == nullptr)
		reds = ResourceUtil::getResourceImageLocal("dom/ControlPointSprite_redC.png");
	return reds;
}

static std::shared_ptr<BitmapData> get_nones()
{
	if (nones == nullptr)
		nones = ResourceUtil::getResourceImageLocal("dom/ControlPointSprite_noneC.png");
	return nones;
}

static std::shared_ptr<BitmapData> get_chars()
{
	if (chars == nullptr)
		chars = ResourceUtil::getResourceImageLocal("dom/ControlPointSprite_charsC.png");
	return chars;
}


ControlPointSprite::ControlPointSprite(const TT &t, int pointId) :
	Sprite3D(TT(t.type, this->TYPE), 0, 0, nullptr),
	__type_next(t.type_next)
{
	std::shared_ptr<BitmapData> nones = get_nones();
	this->width = nones->width();
	this->height = nones->height();
	this->currChar = pointId;
	this->progress = 0;
	this->last_progress = 1000;

	material = this->tm = new_TextureMaterial();
	this->tm->init(false, false);

	int size = 50;
	bound.maxX = bound.maxY = bound.maxZ = -size;
	bound.minX = bound.minY = bound.minZ = size;

	scale.x = scale.y = scale.z = 2;
}


void ControlPointSprite::init()
{
	std::shared_ptr<BitmapData> abds = get_abds();
	red = getTexture(get_reds().get(), abds.get());
	blue = getTexture(get_blues().get(), abds.get());
	none = getTexture(get_nones().get(), abds.get());
}

void ControlPointSprite::_destroy()
{
	//red->dispose();
	//blue->dispose();
	//none->dispose();
	red = blue = none = nullptr;
}

std::shared_ptr<BitmapData> ControlPointSprite::getTexture(BitmapData *in, BitmapData *abds)
{
	std::shared_ptr<BitmapData> bitmapData = std::make_shared<BitmapData>(256, 256, true);

	for (int x = 0; x < 256; x++)
	{
		for (int y = 0; y < 256; y++)
		{
			uint32_t _abds = abds->getPixel(x, y);
			uint32_t alpha = _abds & 0xFF;
			double a = alpha * 0.00392156862745098;
			uint32_t color = in->getPixel(x, y);
			uint32_t RED = ((color >> 16) & 0xFF) * a;
			uint32_t GREEN = ((color >> 8) & 0xFF) * a;
			uint32_t BLUE = (color & 0xFF) * a;
			uint32_t result = (alpha << 24) | (RED << 16) | (GREEN << 8) | BLUE;
			bitmapData->setPixel(x, y, result);
		}
	}

	return bitmapData;
}

void ControlPointSprite::drawChar(BitmapData *out)
{
	QPainter painter(out->qimage());
	//painter.drawImage((96 - (64 * this->currChar)), 96);
	painter.drawImage(96, 96, *get_chars()->qimage(), 64 * this->currChar, 0, 64, 64);
	painter.end();
}

static void drawMask(BitmapData *in, BitmapData *in_none, BitmapData *out, double value)
{
	int width = in_none->width();
	int height = in_none->height();

	if (value < 0)
		value = -value;

	QPainter painter(out->qimage());
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setPen(QPen(Qt::NoPen));

	painter.setBrush(QBrush(*in->qimage()));
	painter.drawPie(0, 0, width, height, 1440, ((value * 3.6) * 16));

	double last = 5760 - ((value * 3.6) * 16);

	painter.setBrush(QBrush(*in_none->qimage()));
	painter.drawPie(0, 0, width, height, 1440 + ((value * 3.6) * 16), last);

	painter.end();
}



void ControlPointSprite::redraw()
{
	if (this->progress <= -100)
	{
		this->progress = -100;
	}
	else
	{
		if (this->progress >= 100)
		{
			this->progress = 100;
		}
	}

	if (this->progress == this->last_progress)
		return;

	this->last_progress = this->progress;


	int step = (int(this->progress) / 2) + 100;


	//std::shared_ptr<BitmapTextureResource> resource = resources.value(step);
	std::shared_ptr<BitmapTextureResource> resource = resources[step];

	if (resource == nullptr)
	{
		std::shared_ptr<BitmapData> bitmapData = std::make_shared<BitmapData>(width, height, true, 0);
		resource = std::make_shared<BitmapTextureResource>(bitmapData, false, false, false);
		//resources.insert(step, resource);
		resources[step] = resource;

		this->tm->setTextureResource(resource);

		bitmapData->fill(0);





		if (this->progress == BALANCE_NONE)
		{
			bitmapData->draw(none.get());
			this->drawChar(bitmapData.get());
			return;
		}

		if (this->progress == BALANCE_RED)
		{
			bitmapData->draw(red.get());
			this->drawChar(bitmapData.get());
			return;
		}

		if (this->progress == BALANCE_BLUE)
		{
			bitmapData->draw(blue.get());
			this->drawChar(bitmapData.get());
			return;
		}

		if (this->progress < 0)
		{
			drawMask(red.get(), none.get(), bitmapData.get(), this->progress);
			this->drawChar(bitmapData.get());
		}
		else
		{
			drawMask(blue.get(), none.get(), bitmapData.get(), this->progress);
			this->drawChar(bitmapData.get());
		}
	}
	else
	{
		this->tm->setTextureResource(resource);
	}
}


std::shared_ptr<ControlPointSprite> new_ControlPointSprite(int pointId)
{
	return std::make_shared<ControlPointSprite>(Object3D::TT(ControlPointSprite::TYPE), pointId);
}
