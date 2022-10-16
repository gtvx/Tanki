#include "paintmaterial.h"
#include "alternativa/engine3d/materials/textureresourcesregistry.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/gfx/core/bitmaptextureresource.h"
#include "flash/display3D/Context3DVertexBufferFormat.h"
#include "flash/display3D/Context3DProgramType.h"
#include "alternativa/engine3d/core/camera3d.h"
#include "alternativa/gfx/core/device.h"
#include "alternativa/engine3d/core/view.h"
#include "alternativa/engine3d/core/shadowmap.h"
#include "alternativa/gfx/core/rendertargettextureresource.h"
#include "alternativa/engine3d/materials/paintvertexshader.h"
#include "flash/display/stage3d.h"
#include "flash/display3D/context3d.h"
#include "mymath.h"
#include "file.h"
#include "flash/display/bitmapdata.h"
#include "flash/display3D/textures/texture.h"



const Material::T *PaintMaterial::TYPE = new Material::T("PaintMaterial");


PaintMaterial::PaintMaterial(const T *type,
							 std::shared_ptr<BitmapData> spriteSheetBitmap,
							 std::shared_ptr<BitmapData> lightMapBitmap,
							 std::shared_ptr<BitmapData> param3,
							 MipMapping mipMapping) :
	TextureMaterial(type)
{
	TextureMaterial::init(true, true, mipMapping);
	TextureMaterial::setTexture(param3);


	//this->fragConst = Vector.<Number>([0, 0.5, 1, 2, 0.999, 0.999, 0, 0]);
	fragConst[0] = 0;
	fragConst[1] = 0.5;
	fragConst[2] = 1;
	fragConst[3] = 2;
	fragConst[4] = 0.999;
	fragConst[5] = 0.999;
	fragConst[6] = 0;
	fragConst[7] = 0;

	_mipMapping = MipMapping::OBJECT_DISTANCE;

	this->spriteSheetBitmap = spriteSheetBitmap;
	this->lightMapBitmap = lightMapBitmap;

	this->_spriteSheetResource = TextureResourcesRegistry::getTextureResource(spriteSheetBitmap, _mipMapping != MipMapping::NONE, true, false);
	this->_lightMapResource = TextureResourcesRegistry::getTextureResource(lightMapBitmap, _mipMapping != MipMapping::NONE, true, false);

	this->spriteSheetResource = this->_spriteSheetResource.get();
	this->lightMapResource = this->_lightMapResource.get();

	uvTransformConst[0] = double(param3->width()) / double(spriteSheetBitmap->width());
	uvTransformConst[5] = double(param3->height()) / double(spriteSheetBitmap->height());
	_mipMapping = (!(!(this->spriteSheetResource->mipMapping()))) ? MipMapping::PER_PIXEL : MipMapping::NONE;

	_mipMapping = MipMapping::OBJECT_DISTANCE;
}

bool PaintMaterial::transparent()
{
	return false;
}

void PaintMaterial::drawOpaque(Camera3D *param1, VertexBufferResource *vertex, IndexBufferResource *index, int param4, int param5, Object3D *param6)
{
	Device *device = param1->device;

	BitmapData *_loc7_ = this->getTexturePtr();

	if (_loc7_ == nullptr)
	{
		return;
	}

	PaintMaterial *_this = this;

	bool _loc9_ = ((param1->fogAlpha > 0) && (param1->fogStrength > 0));
	bool _loc10_ = ((((!(param1->view->constrained)) && (param1->ssao)) && (param1->ssaoStrength > 0)) && (param6->useDepth));
	bool _loc11_ = ((((!(param1->view->constrained)) && (!(param1->directionalLight == nullptr))) && (param1->directionalLightStrength > 0)) && (param6->useLight));
	bool _loc12_ = (((((!(param1->view->constrained)) && (!(param1->shadowMap == nullptr))) && (param1->shadowMapStrength > 0)) && (param6->useLight)) && (param6->useShadowMap));
	bool _loc13_ = (((((!(param1->view->constrained)) && (param1->deferredLighting)) && (param1->deferredLightingStrength > 0)) && (param6->useDepth)) && (param6->useLight));
	bool _loc14_ = ((alphaTestThreshold > 0) && (_this->transparent()));

	device->setProgram(_this->getProgram(!_loc14_, false, false, false, param1->view->quality, repeat, _mipMapping != MipMapping::NONE, (!(param6->concatenatedColorTransform == nullptr)), false, _loc9_, false, _loc10_, _loc11_, _loc12_, (_loc7_ == nullptr), false, _loc13_, false, param1->view->correction, (!(param6->concatenatedBlendMode == BlendMode::NORMAL)), _loc14_, false));


	device->setTextureAt(0, _this->textureResource);


	double uvCorrection[4];
	uvCorrection[0] = _this->textureResource->correctionU();
	uvCorrection[1] = _this->textureResource->correctionV();
	uvCorrection[2] = 0;
	uvCorrection[3] = 0;


	if (_loc10_)
	{
		device->setTextureAt(1, param1->depthMap);
	}
	else
	{
		device->setTextureAt(1, nullptr);
	}



	if (_loc12_)
	{
		device->setTextureAt(2, param1->shadowMap->map);
		device->setTextureAt(3, param1->shadowMap->noise.get());
	}
	else
	{
		device->setTextureAt(2, nullptr);
		device->setTextureAt(3, nullptr);
	}

	device->setTextureAt(4, _this->spriteSheetResource);
	device->setTextureAt(6, _this->lightMapResource);



	if (_loc13_)
	{
		device->setTextureAt(5, param1->lightMap);
	}
	else
	{
		device->setTextureAt(5, nullptr);
	}

	device->setVertexBufferAt(0, vertex, 0, Context3DVertexBufferFormat::FLOAT_3);
	device->setVertexBufferAt(1, vertex, 3, Context3DVertexBufferFormat::FLOAT_2);
	device->setVertexBufferAt(2, nullptr);


	if (_loc11_)
	{
		device->setVertexBufferAt(2, vertex, 5, Context3DVertexBufferFormat::FLOAT_3);
	}
	else
	{
		device->setVertexBufferAt(2, nullptr);
	}





	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 0, param6->transformConst, 3, false);
	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 4, uvCorrection, 1);
	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 14, _this->uvTransformConst, 2);


	device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 23, this->fragConst, 2, false);

	if (param6->concatenatedColorTransform != nullptr)
	{
		device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 0, param6->colorConst, 2, false);
	}

	if (_loc14_)
	{
		fragmentConst[3] = alphaTestThreshold;
		device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 14, fragmentConst, 1, false);
	}

	device->drawTriangles(index, param4, param5);


	param1->numDraws++;
	param1->numTriangles = param1->numTriangles + param5;
}


void PaintMaterial::drawTransparent(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *index, int param4, int param5, Object3D *param6, bool param7)
{
	BitmapData *_loc8_ = getTexturePtr();
	if (_loc8_ == nullptr) //&& (_textureATF == null)
	{
		return;
	}

	Device *_loc9_ = param1->device;
	bool _loc10_ = ((param1->fogAlpha > 0) && (param1->fogStrength > 0));
	bool _loc11_ = ((((!(param1->view->constrained)) && (param1->softTransparency)) && (param1->softTransparencyStrength > 0)) && (param6->softAttenuation > 0));
	bool _loc12_ = ((((!(param1->view->constrained)) && (param1->ssao)) && (param1->ssaoStrength > 0)) && (param6->useDepth));
	bool _loc13_ = ((((!(param1->view->constrained)) && (!(param1->directionalLight == nullptr))) && (param1->directionalLightStrength > 0)) && (param6->useLight));
	bool _loc14_ = (((((!(param1->view->constrained)) && (!(param1->shadowMap == nullptr))) && (param1->shadowMapStrength > 0)) && (param6->useLight)) && (param6->useShadowMap));
	bool _loc15_ = (((!(param1->view->constrained)) && (param1->deferredLighting)) && (param1->deferredLightingStrength > 0));
	bool _loc16_ = (((_loc15_) && (param6->useDepth)) && (param6->useLight));
	_loc9_->setProgram(this->getProgram(false, false, false, false, param1->view->quality, repeat, _mipMapping != MipMapping::NONE, (!(param6->concatenatedColorTransform == nullptr)), (param6->concatenatedAlpha < 1), _loc10_, _loc11_, _loc12_, _loc13_, _loc14_, (_loc8_ == nullptr), ((_loc8_ == nullptr) && (!(_textureATFAlpha == nullptr))), _loc16_, false, param1->view->correction, (!(param6->concatenatedBlendMode == BlendMode::NORMAL)), false, param7));
	_loc9_->setTextureAt(0, textureResource);
	uvCorrection[0] = textureResource->correctionU();
	uvCorrection[1] = textureResource->correctionV();

	if (((_loc12_) || (_loc11_)))
	{
		_loc9_->setTextureAt(1, param1->depthMap);
	}
	else
	{
		_loc9_->setTextureAt(1, nullptr);
	}

	if (_loc14_)
	{
		_loc9_->setTextureAt(2, param1->shadowMap->map);
		_loc9_->setTextureAt(3, param1->shadowMap->noise.get());
	}
	else
	{
		_loc9_->setTextureAt(2, nullptr);
		_loc9_->setTextureAt(3, nullptr);
	}

	_loc9_->setTextureAt(4, this->spriteSheetResource);
	_loc9_->setTextureAt(6, this->lightMapResource);

	if (_loc16_)
	{
		_loc9_->setTextureAt(5, param1->lightMap);
	}
	else
	{
		_loc9_->setTextureAt(5, nullptr);
	}

	_loc9_->setVertexBufferAt(0, param2, 0, Context3DVertexBufferFormat::FLOAT_1);
	_loc9_->setVertexBufferAt(1, nullptr);
	_loc9_->setVertexBufferAt(2, nullptr);
	_loc9_->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 0, param6->transformConst, 3, false);
	_loc9_->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 4, uvCorrection, 1);
	_loc9_->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 14, uvTransformConst, 2);
	_loc9_->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 23, this->fragConst, 2, false);

	if (_loc11_)
	{
		fragmentConst[2] = param6->softAttenuation;
		_loc9_->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 14, fragmentConst, 1);
	}

	if (param6->concatenatedColorTransform != nullptr)
	{
		_loc9_->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 0, param6->colorConst, 2, false);
	}
	else
	{
		if (param6->concatenatedAlpha < 1)
		{
			_loc9_->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 0, param6->colorConst, 1);
		}
	}

	_loc9_->drawTriangles(index, param4, param5);
	param1->numDraws++;
	param1->numTriangles = (param1->numTriangles + param5);
}

void PaintMaterial::disposeResource()
{
	if (textureResource != nullptr)
	{
		this->textureResource->dispose();
		this->textureResource = nullptr;
		this->_textureResource = nullptr;
	}
	if (this->spriteSheetResource != nullptr)
	{
		this->spriteSheetResource->dispose();
		this->spriteSheetResource = nullptr;
		this->_spriteSheetResource = nullptr;
	}
	if (this->lightMapResource != nullptr)
	{
		this->lightMapResource->dispose();
		this->lightMapResource = nullptr;
		this->_lightMapResource = nullptr;
	}
}

void PaintMaterial::dispose()
{
	this->disposeResource();
	this->spriteSheetBitmap = nullptr;
	this->lightMapBitmap = nullptr;
}

ProgramResource *PaintMaterial::getProgram(bool param1, bool param2, bool param3, bool param4, bool param5, bool param6, bool param7, bool param8, bool param9, bool param10, bool param11, bool param12, bool param13, bool param14, bool param15, bool param16, bool param17, bool param18, bool param19, bool param20, bool param21, bool param22)
{
	qDebug("used PaintMaterial::getProgram()");

	(void)param1;
	(void)param2;
	(void)param3;
	(void)param4;
	(void)param5;
	(void)param6;
	(void)param7;
	(void)param8;
	(void)param9;
	(void)param10;
	(void)param11;
	(void)param12;
	(void)param12;
	(void)param13;
	(void)param14;
	(void)param15;
	(void)param16;
	(void)param17;
	(void)param18;
	(void)param19;
	(void)param20;
	(void)param21;
	(void)param22;


	/*
	int _loc23_ = (((((((((((((((((((((int(param1) | (int(param2) << 1)) | (int(param3) << 2)) | (int(param4) << 3)) | (int(param5) << 4)) | (int(param6) << 5)) | (int(param7) << 6)) | (int(param8) << 7)) | (int(param9) << 8)) | (int(param10) << 9)) | (int(param11) << 10)) | (int(param12) << 11)) | (int(param13) << 12)) | (int(param14) << 13)) | (int(param15) << 14)) | (int(param16) << 15)) | (int(param17) << 16)) | (int(param18) << 17)) | (int(param19) << 18)) | (int(param20) << 19)) | (int(param21) << 20)) | (int(param22) << 21));
	ProgramResource *_loc24_ = programs[_loc23_];
	if (_loc24_ == nullptr)
	{
		QByteArray _loc25_ = PaintVertexShader((!(param22)), ((((param14) || (param11)) || (param12)) || (param17)), param13, param4, param14, param10, param2, param3, param3, param19).agalcode;
		QByteArray _loc26_ = PaintFragmentShader(param6, param5, param7, param15, param21, (((!(param1)) && (!(param16))) && (!(param15))), param8, param9, param3, param13, param11, param12, param17, param18, param14, param10, param2, param20).agalcode;
		_loc24_ = ProgramResource(_loc25_, _loc26_);
		this->programs[_loc23_] = _loc24_;
	}
	return _loc24_;
	*/
	return nullptr;
}



std::shared_ptr<PaintMaterial> new_PaintMaterial(std::shared_ptr<BitmapData> spriteSheetBitmap,
												 std::shared_ptr<BitmapData> lightMapBitmap,
												 std::shared_ptr<BitmapData> param3,
												 MipMapping mipMapping)
{
	auto p = std::make_shared<PaintMaterial>(PaintMaterial::TYPE,
											 spriteSheetBitmap,
											 lightMapBitmap,
											 param3,
											 mipMapping);
	p->ptr = p;
	return p;
}
