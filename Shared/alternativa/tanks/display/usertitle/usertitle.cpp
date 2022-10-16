#include "usertitle.h"
#include "alternativa/engine3d/objects/sprite3d.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/engine3d/core/Clipping.h"
#include "flash/utils/gettimer.h"
#include "progressbarskin.h"
#include "usertitleprogressbar.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/engine3d/core/object3dcontainer.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "flash/geom/matrix.h"
#include "alternativa/resource/stubbitmapdata.h"
#include "alternativa/engine3d/core/MipMapping.h"
#include "resource/resourceutil.h"
#include <QImage>
#include <QPainter>
#include "alternativa/gfx/core/bitmaptextureresource.h"
#include "flash/display3D/textures/texture.h"
#include "effectindicator.h"
#include "alternativa/tanks/models/inventory/InventoryItemType.h"
#include <QDebug>


static const int EFFECT_WARNING_TIME = 3000;
static const int MAX_HEALTH = 10000;
static const int RANK_ICON_SIZE = 13;
static const int LABEL_HEIGHT = 15;
static const int RANK_ICON_OFFSET_Y = 2;
static const int EFFECTS_ICON_SIZE = 16;
static const int LABEL_SPACING = 3;
static const int HEALTH_BAR_SPACING_Y = 2;
static const int WEAPON_BAR_SPACING_Y = -1;
static const int EFFECTS_SPACING_Y = 4;
static const int EFFECTS_SPACING_X = 4;
static const int BAR_WIDTH = 100;
static const int BAR_HEIGHT = 8;
static const int TEXTURE_MARGIN = 3;
static const int TEXTURE_MARGIN_2 = (2 * TEXTURE_MARGIN);//6

//private static const inventoryItemTypes:Vector.<int> = Vector.<int>([InventoryItemType.FISRT_AID, InventoryItemType.ARMOR, InventoryItemType.POWER, InventoryItemType.SPEED]);
//private static const filter:GlowFilter = new GlowFilter(0, 0.8, 4, 4, 3);

static const int inventoryItemTypes[4] = {InventoryItemType::FISRT_AID, InventoryItemType::ARMOR, InventoryItemType::POWER, InventoryItemType::SPEED};


const int UserTitle::BIT_LABEL = 1;
const int UserTitle::BIT_HEALTH = 2;
const int UserTitle::BIT_WEAPON = 4;
const int UserTitle::BIT_EFFECTS = 8;

static const double ALPHA_SPEED = 0.002;



UserTitle::UserTitle(bool isLocal)
{
	this->hidden = true;
	this->teamType = (BattleTeamType)-1;

	this->material = new_TextureMaterial();
	this->material->init();
	this->local = isLocal;

	this->weaponBar = nullptr;
	this->healthBar = nullptr;
	this->configFlags = 0;
	this->dirtyFlags = 0;
	this->label_textWidth = 0;
	this->effectIndicators[0] = nullptr;
	this->indicatorsNumberChanged = false;
	this->numVisibleIndicators = 0;
	this->effectIndicatorsY = 0;
	this->rankId = 1;
	this->health = 0;
	this->weaponStatus = 0;
	this->time = 0;
	this->_indexFrame = 0;


	font.setFamily("Tahoma");
	font.setPointSizeF(9);
	font.setBold(true);


	this->sprite = new_Sprite3D(100, 100, this->material);
	this->sprite->perspectiveScale = false;
	this->sprite->alpha = 0;
	this->sprite->visible = false;
	this->sprite->shadowMapAlphaThreshold = 100;
	this->sprite->softAttenuation = 0;
	this->sprite->useLight = false;
	this->sprite->useShadowMap = false;
	this->sprite->sorting = 0;
	this->sprite->clipping = Clipping::FACE_CLIPPING;
	this->material->uploadEveryFrame = true;

	this->skin = ProgressBarSkin::getHEALTHBAR_DM();

	if (isLocal == false)
	{
		this->sprite->originY = 1;
	}
	else
	{
		this->sprite->depthTest = false;
	}

	this->hidden = true;
}

BitmapData* UserTitle::getTexture()
{
	return this->texture.get();
}

void UserTitle::hide()
{
	if (this->hidden == true)
		return;
	this->hidden = true;
}

void UserTitle::show()
{
	if (this->hidden == false)
		return;
	this->hidden = false;
}

bool UserTitle::isHidden()
{
	return this->hidden;
}

void UserTitle::setConfiguration(int configFlags)
{
	//configFlags |= BIT_HEALTH;
	//configFlags |= BIT_LABEL;
	if (this->configFlags == configFlags)
	{
		return;
	}
	this->configFlags = configFlags;
	this->updateConfiguration();
}

void UserTitle::setTeamType(BattleTeamType teamType)
{
	if (this->teamType == teamType)
	{
		return;
	}
	this->teamType = teamType;

	switch (teamType)
	{
		case BattleTeamType::RED:
			this->skin = ProgressBarSkin::getHEALTHBAR_RED();
			break;
		case BattleTeamType::BLUE:
			this->skin = ProgressBarSkin::getHEALTHBAR_BLUE();
			break;
		default:
			this->skin = ProgressBarSkin::getHEALTHBAR_DM();
			break;
	}

	const int flags[3] = {BIT_LABEL, BIT_HEALTH, BIT_WEAPON};
	for (int i = 0; i < 3; i++)
	{
		int flag = flags[i];
		if (this->hasOption(flag))
		{
			this->markDirty(flag);
		}
	}
}

void UserTitle::setRank(int rankId)
{
	if (this->rankId == rankId)
	{
		return;
	}
	this->rankId = rankId;
	if (this->hasOption(BIT_LABEL))
	{
		this->markDirty(BIT_LABEL | BIT_HEALTH | BIT_WEAPON | BIT_EFFECTS);
	}
}

void UserTitle::setLabelText(const QString &labelText)
{
	if (this->labelText == labelText)
	{
		return;
	}
	this->labelText = labelText;

	QFontMetrics metrics(font);
	label_textWidth = metrics.tightBoundingRect(labelText).right();

	if (this->hasOption(BIT_LABEL))
	{
		this->updateConfiguration();
		this->markDirty(BIT_LABEL | BIT_HEALTH | BIT_WEAPON | BIT_EFFECTS);
	}
}

void UserTitle::setHealth(int health)
{
	if (this->health == health)
	{
		return;
	}
	this->health = health;
	if (this->hasOption(BIT_HEALTH))
	{
		this->markDirty(BIT_HEALTH);
	}
}

void UserTitle::setWeaponStatus(int weaponStatus)
{
	if (this->weaponStatus == weaponStatus)
	{
		return;
	}
	this->weaponStatus = weaponStatus;
	if (this->hasOption(BIT_WEAPON))
	{
		this->markDirty(BIT_WEAPON);
	}
}

void UserTitle::showIndicator(int effectId, int duration)
{
	if (!this->hasOption(BIT_EFFECTS))
		return;

	EffectIndicator *indicator = this->getEffectIndicatorById(effectId);
	if (indicator != nullptr)
	{
		if (indicator->isHidden())
		{
			this->changeVisibleIndicatorsNumber(1);
		}
		indicator->show(duration);
	}
}

void UserTitle::hideIndicator(int effectId)
{
	if (!this->hasOption(BIT_EFFECTS))
		return;

	EffectIndicator* indicator = this->getEffectIndicatorById(effectId);
	if (indicator != nullptr)
	{
		indicator->hide();
	}
}

void UserTitle::hideIndicators()
{
	if (!this->hasOption(BIT_EFFECTS) || this->effectIndicators[0] == nullptr)
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		this->hideIndicator(inventoryItemTypes[i]);
	}
}

void UserTitle::doHideIndicator(EffectIndicator *indicator)
{
	indicator->clear(this->texture.get());
	this->changeVisibleIndicatorsNumber(-1);
}





void UserTitle::update(const Vector3 *pos)
{
	this->setPosition(pos);
	int now = getTimer();
	int delta = now - this->time;
	this->time = now;
	this->updateVisibility(delta);

	if (this->dirtyFlags != 0)
	{

		std::shared_ptr<BitmapData> bitmap = texture; //std::make_shared<BitmapData>(110, 100, true, 0x00000000);






		if (this->isDirtyAndHasOption(BIT_LABEL))
		{
			this->updateLabel();
		}


		if (this->isDirtyAndHasOption(BIT_HEALTH))
		{
			this->healthBar->setSkin(this->skin);
			this->healthBar->setProgress(this->health);
			this->healthBar->draw((uint32_t*)bitmap->qimage()->bits(), bitmap->qimage()->width());
		}

		if (this->isDirtyAndHasOption(BIT_WEAPON))
		{
			this->weaponBar->setProgress(this->weaponStatus);
			this->weaponBar->draw((uint32_t*)bitmap->qimage()->bits(), bitmap->qimage()->width());
		}


		if (this->isDirtyAndHasOption(BIT_EFFECTS))
		{
			if (this->effectIndicators[0] != nullptr)
			{
				for (int i = 0; i < 4; i++)
					this->effectIndicators[i]->forceRedraw();
			}
		}


		//this->material->setTexture(bitmap);

		BitmapTextureResource *t = this->material->getTextureResource();
		if (t == nullptr)
		{
			std::shared_ptr<BitmapTextureResource> p = std::make_shared<BitmapTextureResource>(bitmap,
																							   this->material->_mipMapping != MipMapping::NONE,
																							   this->material->repeat,
																							   this->material->_hardwareMipMaps);

			this->material->setTextureResource(p);
			t = p.get();
			_indexFrame++;
		}
		else
		{
			_indexFrame++;
			t->updateResource();
		}

		this->dirtyFlags = 0;
	}



	if (this->hasOption(BIT_EFFECTS))
	{
		this->updateEffectIndicators(now, delta);
	}

}

bool UserTitle::isDirtyAndHasOption(int bit)
{
	return ((!((this->dirtyFlags & bit) == 0)) && (!((this->configFlags & bit) == 0)));
}

void UserTitle::updateEffectIndicators(int now, int delta)
{

	if (effectIndicators[0] == nullptr)
		return;

	if (this->indicatorsNumberChanged)
	{
		this->indicatorsNumberChanged = false;
		int x = ((((int(this->textureRect.width) + TEXTURE_MARGIN_2) -
				   (this->numVisibleIndicators * EFFECTS_ICON_SIZE)) - ((this->numVisibleIndicators - 1) * EFFECTS_SPACING_X)) >> 1);

		for (int i = 0; i < 4; i++)
		{
			EffectIndicator *indicator = this->effectIndicators[i];
			if (indicator->isVisible())
			{
				indicator->clear(this->texture.get());
			}
			if (!indicator->isHidden())
			{
				indicator->setPosition(x, this->effectIndicatorsY);
				x = (x + (EFFECTS_ICON_SIZE + EFFECTS_SPACING_X));
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		EffectIndicator *indicator = this->effectIndicators[i];
		indicator->update(now, delta, this->texture.get(), this->material.get());
	}
}

void UserTitle::changeVisibleIndicatorsNumber(int increment)
{
	this->numVisibleIndicators += increment;
	this->indicatorsNumberChanged = true;
}

void UserTitle::updateConfiguration()
{
	if (this->configFlags == 0)
	{
		return;
	}
	this->setupTexture();
	this->setupComponents();
}

void UserTitle::setupTexture()
{
	int width = 0;
	int height = 0;


	if (this->hasOption(BIT_LABEL))
	{
		/*
		if (this->label == nullptr)
		{
			this->createLabelComponents();
		}
		this->label.text = (this->labelText == nullptr) ? "" : this->labelText;
		width = (RANK_ICON_SIZE + LABEL_SPACING) + this->label->textWidth;
		*/

		width = RANK_ICON_SIZE + LABEL_SPACING + label_textWidth;

		height = LABEL_HEIGHT;
	}

	if (this->hasOption(BIT_HEALTH))
	{
		if (width < BAR_WIDTH)
		{
			width = BAR_WIDTH;
		}
		if (height > 0)
		{
			height = (height + HEALTH_BAR_SPACING_Y);
		}
		height = (height + BAR_HEIGHT);
		if (this->hasOption(BIT_WEAPON))
		{
			height = (height + (WEAPON_BAR_SPACING_Y + BAR_HEIGHT));
		}
	}

	if (this->hasOption(BIT_EFFECTS))
	{
		int numEffects = 4;
		int w = ((numEffects * EFFECTS_ICON_SIZE) + ((numEffects - 1) * EFFECTS_SPACING_X));
		if (width < w)
		{
			width = w;
		}
		if (height > 0)
		{
			height = (height + EFFECTS_SPACING_Y);
		}
		height = (height + EFFECTS_ICON_SIZE);
	}

	width = width + (2 * TEXTURE_MARGIN);
	height = height + (2 * TEXTURE_MARGIN);

	if ((((this->texture == nullptr) || (!(this->texture->width() == width))) || (!(this->texture->height() == height))))
	{
		if (this->texture != nullptr)
		{
			this->texture->dispose();
		}
		this->texture = std::make_shared<BitmapData>(width, height, true, 0);
		this->material->setTexture(this->texture);
		this->sprite->width = width;
		this->sprite->height = height;
		this->textureRect.x = 0;
		this->textureRect.y = 0;
		this->textureRect.width = this->texture->width();
		this->textureRect.height = this->texture->height();

		this->markDirty(BIT_LABEL | BIT_HEALTH | BIT_WEAPON | BIT_EFFECTS);
	}
}

void UserTitle::setupComponents()
{
	int top = TEXTURE_MARGIN;
	if (this->hasOption(BIT_LABEL))
	{
		top = top + LABEL_HEIGHT;
	}
	if (this->hasOption(BIT_HEALTH))
	{
		if (this->hasOption(BIT_LABEL))
		{
			top = top + HEALTH_BAR_SPACING_Y;
		}
		int left = (int(this->textureRect.width) - BAR_WIDTH) >> 1;
		this->healthBar = new UserTitleProgressBar(left, top, MAX_HEALTH, BAR_WIDTH, this->skin);
		top = top + BAR_HEIGHT;
		if (this->hasOption(BIT_WEAPON))
		{
			top = top + WEAPON_BAR_SPACING_Y;
			this->weaponBar = new UserTitleProgressBar(left, top, 100, BAR_WIDTH, ProgressBarSkin::getWEAPONBAR());
			top = top + BAR_HEIGHT;
		}
	}
	if (this->hasOption(BIT_EFFECTS))
	{
		top = top + EFFECTS_SPACING_Y;
		this->effectIndicatorsY = top;
		this->createEffectsIndicators();
	}
}


void UserTitle::addToContainer(Scene3DContainer *container)
{
	if (this->sprite->getParent() != nullptr)
	{
		return;
	}
	container->addChild(this->sprite.get());
	this->time = getTimer();
}

void UserTitle::removeFromContainer()
{
	Object3DContainer *container = this->sprite->getParent();
	if (container != nullptr)
	{
		container->removeChild(this->sprite.get());
	}
}

void UserTitle::setPosition(const Vector3 *pos)
{
	this->sprite->position.x = pos->x;
	this->sprite->position.y = pos->y;
	this->sprite->position.z = pos->z;
}

void UserTitle::markDirty(int dirtyBit)
{
	this->dirtyFlags = this->dirtyFlags | dirtyBit;
}

bool UserTitle::hasOption(int optionBit)
{
	return (!((optionBit & this->configFlags) == 0));
}

void UserTitle::createLabelComponents()
{
	/*
	if (this->label == nullptr)
	{
		this->label = new Label();
		this->label.autoSize = TextFieldAutoSize.LEFT;
		this->label.thickness = 50;
	}
	*/
}



void UserTitle::updateLabel()
{
	QPainter painter;
	painter.begin(texture->qimage());

	if (1)
	{
		uint32_t color = this->skin->color;

		QColor colTimeLine(color);
		painter.setPen(colTimeLine);


		painter.setFont(font);

		std::shared_ptr<BitmapData> icon = ResourceUtil::getResourceImageLocal("ranks_small/" + QString::number(this->rankId) + ".png");

		int labelWidth = RANK_ICON_SIZE + LABEL_SPACING + label_textWidth;
		int left = (texture->width() - labelWidth) >> 1;

		int x1 = left;
		int y1 = TEXTURE_MARGIN + RANK_ICON_OFFSET_Y;

		if (icon != nullptr)
			painter.drawImage(x1, y1, *icon->qimage());


		int x = left + RANK_ICON_SIZE + LABEL_SPACING;
		int y = TEXTURE_MARGIN;


		QRect br;
		painter.drawText(x, y, 100, 20, Qt::AlignLeft, labelText, &br);

		/*
		painter.drawText(0, 20, 100, 20, Qt::AlignCenter, QString::number(this->health), &br);

		if (this->isDirtyAndHasOption(BIT_WEAPON))
		{
			painter.drawText(0, 40, 100, 20, Qt::AlignCenter, QString::number(this->weaponStatus), &br);
		}
		*/

	}

#if 0
	BitmapData *tmpBitmapData = this->texture.clone();

	tmpBitmapData->fillRect(this->textureRect, 0);

	int labelWidth = (RANK_ICON_SIZE + LABEL_SPACING) + this->label->textWidth;
	int left = (tmpBitmapData->width() - labelWidth) >> 1;

	Matrix matrix;
	matrix.tx = left;
	matrix.ty = TEXTURE_MARGIN + RANK_ICON_OFFSET_Y;

	rankIcon.rang = this->rankId;

	tmpBitmapData->draw(rankIcon, matrix, nullptr, nullptr, nullptr, true);

	matrix.tx = left + RANK_ICON_SIZE + LABEL_SPACING;
	matrix.ty = TEXTURE_MARGIN;

	this->label->textColor = this->skin.color;

	tmpBitmapData->draw(this->label, &matrix, nullptr, nullptr, nullptr, true);

	//this->texture->applyFilter(tmpBitmapData, this->textureRect, new Point(), filter);

	tmpBitmapData->dispose();
#endif
}

void UserTitle::createEffectsIndicators()
{
	if (effectIndicators[0] != nullptr)
		return;

	for (int i = 0; i < 4; i++)
	{
		int typeId = inventoryItemTypes[i];
		if (typeId == InventoryItemType::FISRT_AID)
		{
			this->effectIndicators[i] = new EffectIndicator(typeId, 100000, this, 300, 0);
		}
		else
		{
			this->effectIndicators[i] = new EffectIndicator(typeId, EFFECT_WARNING_TIME, this, 300, 30);
		}
	}
}

EffectIndicator* UserTitle::getEffectIndicatorById(int effectId)
{
	if (effectIndicators[0] == nullptr)
		return nullptr;

	for (int i = 0; i < 4; i++)
	{
		EffectIndicator *indicator = this->effectIndicators[i];
		if (indicator->effectId() == effectId)
		{
			return indicator;
		}
	}

	return nullptr;
}

void UserTitle::updateVisibility(int delta)
{
	if (this->hidden)
	{
		if (this->sprite->alpha > 0)
		{
			this->sprite->alpha = this->sprite->alpha - (ALPHA_SPEED * delta);
			if (this->sprite->alpha <= 0)
			{
				this->sprite->alpha = 0;
				this->sprite->visible = false;
			}
		}
	}
	else
	{
		this->sprite->visible = true;
		if (this->sprite->alpha < 1)
		{
			this->sprite->alpha = this->sprite->alpha + (ALPHA_SPEED * delta);
			if (this->sprite->alpha > 1)
			{
				this->sprite->alpha = 1;
			}
		}
	}
}

void UserTitle::updateTexture()
{
	this->_indexFrame++;
	BitmapTextureResource *t = this->material->getTextureResource();
	if (t == nullptr)
		return;
	t->updateResource();
}
