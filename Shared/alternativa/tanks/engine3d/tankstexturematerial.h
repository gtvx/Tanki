#ifndef TANKSTEXTUREMATERIAL_H
#define TANKSTEXTUREMATERIAL_H

#include "alternativa/engine3d/materials/texturematerial.h"

class TanksTextureMaterial : public TextureMaterial
{
public:

	static const T *TYPE;

	TanksTextureMaterial(const T *type, std::shared_ptr<BitmapData> aram1 = nullptr, bool param2 = false, bool param3 = true, MipMapping param4 = MipMapping::NONE, double param5 = 1);
	void destroy(bool b);
};

std::shared_ptr<TanksTextureMaterial> new_TanksTextureMaterial(std::shared_ptr<BitmapData> param1, bool param2, bool param3, MipMapping param4, double param5);

#endif // TANKSTEXTUREMATERIAL_H
