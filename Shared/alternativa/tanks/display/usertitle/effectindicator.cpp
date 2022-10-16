#include "effectindicator.h"
#include "alternativa/tanks/sfx/blinker.h"
#include "flash/utils/gettimer.h"
#include "effectindicator.h"
#include "usertitle.h"
#include "resource/resourceutil.h"
#include "alternativa/tanks/models/inventory/InventoryItemType.h"
#include "flash/geom/geomrectangle.h"
#include "flash/display/bitmapdata.h"


static const int STATE_HIDDEN = 1;
static const int STATE_PREPARED = 2;
static const int STATE_VISIBLE = 4;
static const int STATE_HIDING = 8;
//private static const iconHealthCls:Class = EffectIndicator_iconHealthCls;
//private static const iconArmorCls:Class = EffectIndicator_iconArmorCls;
//private static const iconPowerCls:Class = EffectIndicator_iconPowerCls;
//private static const iconNitroCls:Class = EffectIndicator_iconNitroCls;
static const double MIN_ALPHA = 0.2;
//private static var icons:Dictionary;
static GeomRectangle iconRect;
//private static var matrix:Matrix = new Matrix();

static std::shared_ptr<BitmapData> icons[6];

static void initIcons()
{
	icons[InventoryItemType::FISRT_AID] = ResourceUtil::getResourceImageLocal("effect_indicator/health.png");
	icons[InventoryItemType::ARMOR] = ResourceUtil::getResourceImageLocal("effect_indicator/armor.png");
	icons[InventoryItemType::POWER] = ResourceUtil::getResourceImageLocal("effect_indicator/power.png");
	icons[InventoryItemType::SPEED] = ResourceUtil::getResourceImageLocal("effect_indicator/nitro.png");
	BitmapData *bitmap = icons[InventoryItemType::FISRT_AID].get();
	bitmap->getRect(&iconRect);
}

EffectIndicator::EffectIndicator(int effectId, int blinkingTime, UserTitle *userTitle, int initialBlinkInterval, int blinkIntervalDecrement)
{
	if (icons[InventoryItemType::FISRT_AID] == nullptr)
	{
		initIcons();
	}

	this->blinkingStartTime = 0;
	this->needRedraw = false;
	this->blinking = false;
	this->_effectId = effectId;
	this->icon = icons[effectId].get();
	this->blinkingTime = blinkingTime;
	this->userTitle = userTitle;
	this->blinker = new Blinker(initialBlinkInterval, 20, blinkIntervalDecrement, MIN_ALPHA, 1, 10);
	this->state = STATE_HIDDEN;
	this->x = 0;
	this->y = 0;
}

void EffectIndicator::_clear()
{
	for (int i = 0; i < 6; i++)
		icons[i] = nullptr;
}

bool EffectIndicator::isVisible()
{
	return !((this->state & STATE_VISIBLE) == 0);
}

bool EffectIndicator::isHidden()
{
	return this->state == STATE_HIDDEN;
}

void EffectIndicator::show(int duration)
{
	this->state = (this->state & (~(STATE_HIDING)));
	if (((!(this->state == STATE_VISIBLE)) || (!(this->alpha == 1))))
	{
		this->needRedraw = true;
	};
	this->blinkingStartTime = ((getTimer() + duration) - this->blinkingTime);
	this->blinking = false;
	this->alpha = 1;
	if (this->state == STATE_HIDDEN)
	{
		this->state = STATE_PREPARED;
	}
}

void EffectIndicator::hide()
{
	if (this->state == STATE_PREPARED)
	{
		this->userTitle->doHideIndicator(this);
		this->state = STATE_HIDDEN;
		return;
	}
	if ((this->state & (STATE_HIDDEN | STATE_HIDING)) != 0)
	{
		return;
	}
	this->state = this->state | STATE_HIDING;
	this->blinker->setMinValue(0);
	if (!this->blinking)
	{
		this->blinkingStartTime = 0;
		this->blinker->init(getTimer());
		this->blinking = true;
	}
}

void EffectIndicator::clear(BitmapData *texture)
{
	if (this->state == STATE_HIDDEN || this->state == STATE_PREPARED)
		return;

	iconRect.x = this->x;
	iconRect.y = this->y;
	//texture.fillRect(iconRect, 0);

	for (int x = 0; x < iconRect.width; x++)
	{
		for (int y = 0; y < iconRect.height; y++)
		{
			texture->setPixel(this->x + x, this->y + y, 0x00000000);
		}
	}

	userTitle->updateTexture();
}

void EffectIndicator::setPosition(int x, int y)
{
	this->x = x;
	this->y = y;
	this->needRedraw = true;
}

void EffectIndicator::forceRedraw()
{
	this->needRedraw = true;
}

void EffectIndicator::update(int now, int delta, BitmapData *texture, TextureMaterial *sprite)
{
	if (this->state == STATE_HIDDEN)
	{
		return;
	}
	if (this->needRedraw)
	{
		this->draw(texture, sprite);
		this->needRedraw = false;
	}
	if (now > this->blinkingStartTime)
	{
		this->updateBlinking(now, delta, texture, sprite);
	}
	if (this->state == STATE_PREPARED)
	{
		this->state = STATE_VISIBLE;
	}
}

void EffectIndicator::updateBlinking(int now, int delta, BitmapData *texture, TextureMaterial *sprite)
{
	if (this->blinking)
	{
		double newAlpha = this->blinker->updateValue(now, delta);
		if (newAlpha != this->alpha)
		{
			this->alpha = newAlpha;
			this->draw(texture, sprite);
		}
		if (((!((this->state & STATE_HIDING) == 0)) && (this->alpha <= 0)))
		{
			this->userTitle->doHideIndicator(this);
			this->state = STATE_HIDDEN;
		}
	}
	else
	{
		this->blinker->setMinValue(MIN_ALPHA);
		this->blinker->init(now);
		this->blinking = true;
	}
}


void EffectIndicator::draw(BitmapData *texture, TextureMaterial *sprite)
{
	(void)sprite;
	union
	{
		uint32_t color;
		struct
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue;
			uint8_t alpha;
		};
	} u;

	this->clear(texture);

	if (this->alpha != 0)
	{
		for (int x = 0; x < iconRect.width; x++)
		{
			for (int y = 0; y < iconRect.height; y++)
			{
				u.color = this->icon->getPixel(x, y);
				u.alpha = u.alpha * this->alpha;
				texture->setPixel(this->x + x, this->y + y, u.color);
			}
		}
	}

	userTitle->updateTexture();

	//matrix.tx = this->x;
	//matrix.ty = this->y;
	//this->colorTransform.alphaMultiplier = this->alpha;
	//texture.draw(this->icon, matrix, this->colorTransform, null, null, true);
	//sprite.texture = texture;
}
