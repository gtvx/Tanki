#include "tankstexturematerial.h"

const Material::T *TanksTextureMaterial::TYPE = new Material::T("TanksTextureMaterial");


TanksTextureMaterial::TanksTextureMaterial(const T *type, std::shared_ptr<BitmapData> param1, bool param2, bool param3, MipMapping param4, double param5) :
	TextureMaterial(type)
{
	TextureMaterial::init(param2, param3, param4, param5);
	TextureMaterial::setTexture(param1);

	//TYPE type, BitmapData *bitmapData, bool repeat, bool smooth, MipMapping mipMapping, double resolution

	/*
	var bfModel:BattlefieldModel = BattlefieldModel(Main.osgi.getService(IBattleField));
	if (bfModel.toDestroy.indexOf(this) == -1)
	{
		bfModel.toDestroy.push(this);
	}
	*/
}

void TanksTextureMaterial::destroy(bool)
{
	TextureMaterial::dispose();
}

std::shared_ptr<TanksTextureMaterial> new_TanksTextureMaterial(std::shared_ptr<BitmapData> param1, bool param2, bool param3, MipMapping param4, double param5)
{
	auto p = std::make_shared<TanksTextureMaterial>(TanksTextureMaterial::TYPE, param1, param2, param3, param4, param5);
	p->ptr = p;
	return p;
}
