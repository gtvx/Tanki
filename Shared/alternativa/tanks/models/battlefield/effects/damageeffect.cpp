#include "damageeffect.h"
#include "service.h"
#include "alternativa/tanks/models/battlefield/IBattleField.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include <QImage>
#include <QPainter>
#include "flash/display/bitmapdata.h"
#include "alternativa/tanks/models/battlefield/effects/damageupeffect.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "alternativa/tanks/vehicles/tanks/tankskin.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "alternativa/tanks/services/objectpool/objectpoolservice.h"
#include "alternativa/gfx/core/bitmaptextureresource.h"
#include <QHash>

namespace
{
	struct Value
	{
		int damage;
		uint32_t color;

		void operator=(const Value& d)
		{
			this->damage = d.damage;
			this->color = d.color;
		}
	};

	inline bool operator==(const Value& d1, const Value& d2)
	{
		return d1.damage == d2.damage && d1.color == d2.color;
	}

	inline uint32_t qHash(const Value& mc)
	{
		return mc.color + mc.damage;
	}
}

class DamageTextures
{
public:
	QHash<Value, std::shared_ptr<TextureMaterial>> list;

	std::shared_ptr<TextureMaterial> get(int damage, uint32_t color)
	{
		Value v;
		v.damage = damage;
		v.color = color;
		return list.value(v);
	}

	void insert(int damage, uint32_t color, std::shared_ptr<TextureMaterial> &texture)
	{
		Value v;
		v.damage = damage;
		v.color = color;
		list.insert(v, texture);
	}
};


DamageEffect::DamageEffect()
{
	damageTextures = new DamageTextures;
}

DamageEffect::~DamageEffect()
{
	delete damageTextures;
}

void DamageEffect::createEffect(Tank *tank, int damage, uint32_t color)
{
	Object3D *tankObject = tank->skin->turretMesh;
	double height = 350;

	std::shared_ptr<TextureMaterial> damageTexture = damageTextures->get(damage, color);

	if (damageTexture == nullptr)
	{
/*
		class MyBitmapData : public BitmapData
		{
		public:
			MyBitmapData(uint32_t width, uint32_t height, bool transparent = true, uint32_t fillColor = 0xFFFFFFFF)
				: BitmapData(width, height, transparent, fillColor)
			{
				qDebug() << "MyBitmapData" << this;
			}

			~MyBitmapData()
			{
				qDebug() << "~MyBitmapData" << this;
			}
		};

		class MyBitmapTextureResource : public BitmapTextureResource
		{
		public:
			MyBitmapTextureResource(std::shared_ptr<BitmapData> bitmapData, bool mipMapping, bool stretchNotPowerOf2Textures = false, bool calculateMipMapsUsingGPU = false) :
				BitmapTextureResource(bitmapData, mipMapping, stretchNotPowerOf2Textures, calculateMipMapsUsingGPU)
			{
				qDebug() << "MyBitmapTextureResource" << this;
			}

			virtual ~MyBitmapTextureResource()
			{
				qDebug() << "~MyBitmapTextureResource" << this;
			}
		};
*/

		std::shared_ptr<BitmapData> damageBitmap = std::make_shared<BitmapData>(50, 30, true, 0x00000000);

		bool repeat = false;
		MipMapping mipMapping = MipMapping::PER_PIXEL;
		bool _hardwareMipMaps = false;


		std::shared_ptr<BitmapTextureResource> resource =
				std::make_shared<BitmapTextureResource>(damageBitmap,
														mipMapping != MipMapping::NONE,
														repeat,
														_hardwareMipMaps);


		QPainter painter;
		painter.begin(damageBitmap->qimage());
		painter.setPen(QColor(QRgb(color)));

		QFont font = painter.font();
		font.setPixelSize(18);
		painter.setFont(font);

		QRect br;

		painter.drawText(0, 0, 50, 30, Qt::AlignCenter, QString::number(damage), &br);


		painter.end();

		damageTexture = new_TextureMaterial();
		damageTexture->init(false, true, MipMapping::PER_PIXEL, 1);
		damageTexture->setTextureResource(resource);

		damageTextures->insert(damage, color, damageTexture);
	}

	IBattleField *battle = Service::getIBattleField();

	DamageUpEffect *damageEffect = battle->getObjectPoolService()->getDamageUpEffect();
	damageEffect->init(500, 50, 30, 0, height * 0.8, height * 0.15, 0.75, 0, 0, 125, tankObject, damageTexture, BlendMode::NORMAL);
	battle->addGraphicEffect(damageEffect);
}
