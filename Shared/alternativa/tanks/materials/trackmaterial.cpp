#include "trackmaterial.h"
#include "alternativa/engine3d/materials/textureresourcesregistry.h"
//#include "alternativa/engine3d/materials/texturematerialfragmentshader.h"
#include "alternativa/engine3d/materials/texturematerialfragmentshaderhlsl.h"
//#include "alternativa/engine3d/materials/trackmaterialvertexshader.h"
#include "alternativa/engine3d/materials/trackmaterialvertexshaderhlsl.h"
#include "alternativa/gfx/core/programresource.h"
#include "alternativa/gfx/core/device.h"
#include "alternativa/engine3d/core/camera3d.h"
#include "alternativa/engine3d/core/shadowmap.h"
#include "alternativa/gfx/core/bitmaptextureresource.h"
#include "alternativa/engine3d/core/view.h"
#include "alternativa/gfx/core/rendertargettextureresource.h"
#include <QHash>
#include "mymath.h"
#include "flash/display/stage.h"
#include "flash/display/stage3d.h"
#include "flash/display3D/context3d.h"
#include "hash.h"


const Material::T *TrackMaterial::TYPE = new Material::T("TrackMaterial");

static QHash<int, ProgramResource*> programs;


TrackMaterial::TrackMaterial(const T *type, std::shared_ptr<BitmapData> &bitmapData) :
	TextureMaterial(type)
{
	TextureMaterial::init(true);
	TextureMaterial::setTexture(bitmapData);
	uvMatrixProvider = new UVMatrixProvider();
}

TrackMaterial::~TrackMaterial()
{
	delete uvMatrixProvider;
}

void TrackMaterial::update()
{
	memcpy(uvTransformConst, this->uvMatrixProvider->getValues(), 8 * sizeof(double));
}

bool TrackMaterial::transparent()
{
	return false;
}

void TrackMaterial::drawOpaque(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6)
{
	BitmapData *_loc7_ = this->getTexturePtr();
	if (_loc7_ == nullptr)
	{
		return;
	}

	this->update();
	Device *device = param1->device;
	bool _loc9_ = ((param1->fogAlpha > 0) && (param1->fogStrength > 0));
	bool _loc10_ = ((((!(param1->view->constrained)) && (param1->ssao)) && (param1->ssaoStrength > 0)) && (param6->useDepth));
	bool _loc11_ = ((((!(param1->view->constrained)) && (!(param1->directionalLight == nullptr))) && (param1->directionalLightStrength > 0)) && (param6->useLight));
	bool _loc12_ = (((((!(param1->view->constrained)) && (!(param1->shadowMap == nullptr))) && (param1->shadowMapStrength > 0)) && (param6->useLight)) && (param6->useShadowMap));
	bool _loc13_ = (((((!(param1->view->constrained)) && (param1->deferredLighting)) && (param1->deferredLightingStrength > 0)) && (param6->useDepth)) && (param6->useLight));
	bool _loc14_ = ((alphaTestThreshold > 0) && (this->transparent()));
	device->setProgram(this->getProgram(!_loc14_, false, false, false, param1->view->quality, repeat, _mipMapping != MipMapping::NONE, (!(param6->concatenatedColorTransform == nullptr)), false, _loc9_, false, _loc10_, _loc11_, _loc12_, (_loc7_ == nullptr), false, _loc13_, false, param1->view->correction, (!(param6->concatenatedBlendMode == BlendMode::NORMAL)), _loc14_, false));
	device->setTextureAt(0, textureResource);
	uvCorrection[0] = textureResource->correctionU();
	uvCorrection[1] = textureResource->correctionV();

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

	device->setTextureAt(4, nullptr);
	device->setTextureAt(6, nullptr);
	if (_loc13_)
	{
		device->setTextureAt(5, param1->lightMap);
	}
	else
	{
		device->setTextureAt(5, nullptr);
	}

	device->setVertexBufferAt(0, param2, 0, Context3DVertexBufferFormat::FLOAT_3);
	device->setVertexBufferAt(1, param2, 3, Context3DVertexBufferFormat::FLOAT_2);

	if (_loc11_)
	{
		device->setVertexBufferAt(2, param2, 5, Context3DVertexBufferFormat::FLOAT_3);
	}
	else
	{
		device->setVertexBufferAt(2, nullptr);
	}

	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 0, param6->transformConst, 3, false);
	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 4, uvCorrection, 1);
	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 14, uvTransformConst, 2);

	if (param6->concatenatedColorTransform != nullptr)
	{
		device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 0, param6->colorConst, 2, false);
	}

	if (_loc14_)
	{
		fragmentConst[3] = alphaTestThreshold;
		device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 14, fragmentConst, 1, false);
	}


	device->drawTriangles(param3, param4, param5);
	param1->numDraws++;
	param1->numTriangles = (param1->numTriangles + param5);
}

void TrackMaterial::drawTransparent(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6, bool param7)
{
	BitmapData *_loc8_ = this->getTexturePtr();
	if (_loc8_ == nullptr)
	{
		return;
	}

	Device *device = param1->device;
	bool _loc10_ = ((param1->fogAlpha > 0) && (param1->fogStrength > 0));
	bool _loc11_ = ((((!(param1->view->constrained)) && (param1->softTransparency)) && (param1->softTransparencyStrength > 0)) && (param6->softAttenuation > 0));
	bool _loc12_ = ((((!(param1->view->constrained)) && (param1->ssao)) && (param1->ssaoStrength > 0)) && (param6->useDepth));
	bool _loc13_ = ((((!(param1->view->constrained)) && (!(param1->directionalLight == nullptr))) && (param1->directionalLightStrength > 0)) && (param6->useLight));
	bool _loc14_ = (((((!(param1->view->constrained)) && (!(param1->shadowMap == nullptr))) && (param1->shadowMapStrength > 0)) && (param6->useLight)) && (param6->useShadowMap));
	bool _loc15_ = (((!(param1->view->constrained)) && (param1->deferredLighting)) && (param1->deferredLightingStrength > 0));
	bool _loc16_ = (((_loc15_) && (param6->useDepth)) && (param6->useLight));

	device->setProgram(this->getProgram(false,
										false,
										false,
										false,
										param1->view->quality,
										repeat,
										_mipMapping != MipMapping::NONE,
										(!(param6->concatenatedColorTransform == nullptr)),
										(param6->concatenatedAlpha < 1),
										_loc10_,
										_loc11_,
										_loc12_,
										_loc13_,
										_loc14_,
										(_loc8_ == nullptr),
										((_loc8_ == nullptr) && (!(_textureATFAlpha == nullptr))),
										_loc16_,
										false,
										param1->view->correction,
										(!(param6->concatenatedBlendMode == BlendMode::NORMAL)),
										false,
										param7));

	device->setTextureAt(0, textureResource);
	uvCorrection[0] = textureResource->correctionU();
	uvCorrection[1] = textureResource->correctionV();



	if (_loc12_ || _loc11_)
	{
		device->setTextureAt(1, param1->depthMap);
	}
	else
	{
		device->setTextureAt(1, nullptr);
	}

	if (_loc14_)
	{
		device->setTextureAt(2, param1->shadowMap->map);
		device->setTextureAt(3, param1->shadowMap->noise.get());
	}
	else
	{
		device->setTextureAt(2, nullptr);
		device->setTextureAt(3, nullptr);
	}

	device->setTextureAt(4, nullptr);
	device->setTextureAt(6, nullptr);

	if (_loc16_)
	{
		device->setTextureAt(5, param1->lightMap);
	}
	else
	{
		device->setTextureAt(5, nullptr);
	}

	device->setVertexBufferAt(0, param2, 0, Context3DVertexBufferFormat::FLOAT_1);
	device->setVertexBufferAt(1, nullptr);


	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 0, param6->transformConst, 3, false);
	device->setProgramConstantsFromVector(Context3DProgramType::VERTEX, 4, uvCorrection, 1);

	if (_loc11_)
	{
		fragmentConst[2] = param6->softAttenuation;
		device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 14, fragmentConst, 1);
	}

	if (param6->concatenatedColorTransform != nullptr)
	{
		device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 0, param6->colorConst, 2, false);
	}
	else
	{
		if (param6->concatenatedAlpha < 1)
		{
			device->setProgramConstantsFromVector(Context3DProgramType::FRAGMENT, 0, param6->colorConst, 1);
		}
	}

	device->drawTriangles(param3, param4, param5);
	param1->numDraws++;
	param1->numTriangles = param1->numTriangles + param5;	
}


ProgramResource* TrackMaterial::getProgram(bool param1, bool param2, bool param3, bool param4, bool param5, bool param6, bool param7, bool param8, bool param9, bool param10, bool param11, bool param12, bool param13, bool param14, bool param15, bool param16, bool param17, bool param18, bool param19, bool param20, bool param21, bool param22)
{
	int key = (((((((((((((((((((((int(param1) | (int(param2) << 1)) | (int(param3) << 2)) | (int(param4) << 3)) | (int(param5) << 4)) | (int(param6) << 5)) | (int(param7) << 6)) | (int(param8) << 7)) | (int(param9) << 8)) | (int(param10) << 9)) | (int(param11) << 10)) | (int(param12) << 11)) | (int(param13) << 12)) | (int(param14) << 13)) | (int(param15) << 14)) | (int(param16) << 15)) | (int(param17) << 16)) | (int(param18) << 17)) | (int(param19) << 18)) | (int(param20) << 19)) | (int(param21) << 20)) | (int(param22) << 21));
	ProgramResource *programResource = programs[key];
	if (programResource == nullptr)
	{
		//ShaderSampler sampler;
		//QByteArray vertexShaderData = TrackMaterialVertexShader((!(param22)), ((((param14) || (param11)) || (param12)) || (param17)), param13, param4, param14, param10, param2, param3, param3, param19);
		//QByteArray fragmentShaderData = TextureMaterialFragmentShader(sampler, param6, param5, param7, param15, param16, param21, (((!(param1)) && (!(param16))) && (!(param15))), param8, param9, param3, param13, param11, param12, param17, param18, param14, param10, param2, param20);


		QByteArray vertexShaderData_hlsl = TrackMaterialVertexShaderHLSL(!param22,
																		 (((param14 || param11) || param12) || param17),
																		 param13,
																		 param4,
																		 param14,
																		 param10,
																		 param2,
																		 param3,
																		 param3,
																		 param19);

		ShaderSampler sampler2;
		QByteArray fragmentShaderData_hlsl = TextureMaterialFragmentShaderHLSL(sampler2, param6, param5, param7, param15, param16, param21,
																	  (((!(param1)) && (!(param16))) && (!(param15))),
																	  param8, param9, param3, param13, param11, param12,
																	  param17, param18, param14, param10, param2, param20);

		//programResource = new ProgramResource(ProgramResource::TYPE::AGAL, vertexShaderData, fragmentShaderData, sampler);
		programResource = new ProgramResource(9, ProgramResource::TYPE::HLSL, vertexShaderData_hlsl, fragmentShaderData_hlsl, sampler2);
		programs[key] = programResource;
	}
	return programResource;
}

std::shared_ptr<TrackMaterial> new_TrackMaterial(std::shared_ptr<BitmapData> &bitmapData)
{
	auto p = std::make_shared<TrackMaterial>(TrackMaterial::TYPE, bitmapData);
	p->ptr = p;
	return p;
}
