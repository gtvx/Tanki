#include "animatedpaintmaterial.h"
#include "flash/display/bitmapdata.h"
//#include "alternativa/engine3d/materials/paintvertexshader.h"
#include "alternativa/engine3d/materials/paintvertexshaderhlsl.h"
//#include "alternativa/engine3d/materials/animatedpaintfragmentshader.h"
#include "alternativa/engine3d/materials/animatedpaintfragmentshaderhlsl.h"
#include "flash/utils/gettimer.h"
#include "alternativa/engine3d/materials/textureresourcesregistry.h"
#include "alternativa/gfx/core/programresource.h"
#include "alternativa/gfx/shadersampler/shadersampler.h"
#include "mymath.h"
#include <QHash>
#include "hash.h"
#include <QDebug>


const Material::T *AnimatedPaintMaterial::TYPE = new Material::T("AnimatedPaintMaterial");
static QHash<int, ProgramResource*> programs;



AnimatedPaintMaterial::AnimatedPaintMaterial(const T *type,
											 std::shared_ptr<BitmapData> &param1,
											 std::shared_ptr<BitmapData> &param2,
											 std::shared_ptr<BitmapData> &param3,
											 int numFramesX,
											 int numFramesY,
											 int fps,
											 int numFrames,
											 MipMapping mipMapping) :
	PaintMaterial(type, param1, param2, param3, mipMapping)
{
	this->lastFrame = -1;


	this->time = 0;
	this->numFramesX = numFramesX;
	this->numFramesY = numFramesY;
	this->fps = fps;
	this->numFrames = numFrames;
	this->currentFrame = 0;
	this->scaleX = double(param3->width()) / double(param1->width());
	this->scaleY = double(param3->height()) / double(param1->height());
	this->frameWidth = 1. / double(numFramesX);
	this->frameHeight = 1. / double(numFramesY);

	//fragConst = Vector.<Number>([0, 0.5, 1, 2, this->frameWidth, this->frameHeight, 0, 0]);
	fragConst[0] = 0;
	fragConst[1] = 0.5;
	fragConst[2] = 1;
	fragConst[3] = 2;
	fragConst[4] = this->frameWidth;
	fragConst[5] = this->frameHeight;
	fragConst[6] = 0;
	fragConst[7] = 0;
}

void AnimatedPaintMaterial::update()
{
	int _loc1_ = getTimer();
	int _loc2_ = _loc1_ - this->time;
	this->time = _loc1_;
	this->currentFrame = this->currentFrame + ((_loc2_ / 1000.) * this->fps);

	this->currentFrame = MyMath::fmod(this->currentFrame, this->numFrames); //this->currentFrame % this->numFrames;


	if (this->lastFrame == this->currentFrame)
	{
		return;
	}

	//this->numFramesX = 0
	//this->numFramesY = 0
	//this->currentFrame = 14.235999997705221

	int _loc3_;
	int _loc4_;
	_loc3_ = MyMath::fmod(this->currentFrame, this->numFramesX); //this->currentFrame % this->numFramesX;
	_loc4_ = int((this->currentFrame / this->numFramesY));

	this->lastFrame = this->currentFrame;
	uvTransformConst[0] = this->scaleX;
	uvTransformConst[1] = 0;
	uvTransformConst[2] = _loc3_ * this->frameWidth;
	uvTransformConst[3] = 0;
	uvTransformConst[4] = 0;
	uvTransformConst[5] = this->scaleY;
	uvTransformConst[6] = _loc4_ * this->frameHeight;
	uvTransformConst[7] = 0;
	fragConst[6] = uvTransformConst[2];
	fragConst[7] = uvTransformConst[6];
}

void AnimatedPaintMaterial::drawOpaque(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6)
{
	this->update();
	PaintMaterial::drawOpaque(param1, param2, param3, param4, param5, param6);
}

void AnimatedPaintMaterial::drawTransparent(Camera3D *param1, VertexBufferResource *param2, IndexBufferResource *param3, int param4, int param5, Object3D *param6, bool param7)
{
	PaintMaterial::drawTransparent(param1, param2, param3, param4, param5, param6, param7);
}






ProgramResource* AnimatedPaintMaterial::getProgram(bool param1, bool param2, bool param3, bool param4, bool param5, bool param6, bool param7, bool param8, bool param9, bool param10, bool param11, bool param12, bool param13, bool param14, bool param15, bool param16, bool param17, bool param18, bool param19, bool param20, bool param21, bool param22)
{
	int key = (((((((((((((((((((((int(param1) | (int(param2) << 1)) | (int(param3) << 2)) | (int(param4) << 3)) | (int(param5) << 4)) | (int(param6) << 5)) | (int(param7) << 6)) | (int(param8) << 7)) | (int(param9) << 8)) | (int(param10) << 9)) | (int(param11) << 10)) | (int(param12) << 11)) | (int(param13) << 12)) | (int(param14) << 13)) | (int(param15) << 14)) | (int(param16) << 15)) | (int(param17) << 16)) | (int(param18) << 17)) | (int(param19) << 18)) | (int(param20) << 19)) | (int(param21) << 20)) | (int(param22) << 21));
	ProgramResource *programResource = programs[key];
	if (programResource == nullptr)
	{
		//qDebug() << param1 << param2 << param3 << param4 << param5 << param6 << param7 << param8 << param9 << param10 <<
		//param11 << param12 << param13 << param14 << param15 << param16 << param17 << param18 << param19 << param20 << param21 << param22;

		//ShaderSampler sampler;
		//QByteArray vertexShaderData = PaintVertexShader((!(param22)), ((((param14) || (param11)) || (param12)) || (param17)), param13, param4, param14, param10, param2, param3, param3, param19);
		//QByteArray fragmentShaderData = AnimatedPaintFragmentShader(sampler, param6, param5, param7, param15, param21, (((!(param1)) && (!(param16))) && (!(param15))), param8, param9, param3, param13, param11, param12, param17, param18, param14, param10, param2, param20);


		QByteArray vertexShaderData_hlsl = PaintVertexShaderHLSL(!param22,
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
		QByteArray fragmentShaderData_hlsl = AnimatedPaintFragmentShaderHLSL(sampler2, param6, param5, param7, param15, param21,
																			 (((!(param1)) && (!(param16))) && (!(param15))),
																			 param8, param9, param3, param13, param11, param12,
																			 param17, param18, param14, param10, param2, param20);



		//programResource = new ProgramResource(ProgramResource::TYPE::AGAL, vertexShaderData, fragmentShaderData, sampler);
		programResource = new ProgramResource(8, ProgramResource::TYPE::HLSL, vertexShaderData_hlsl, fragmentShaderData_hlsl, sampler2);
		programs[key] = programResource;
	}
	return programResource;
}

std::shared_ptr<AnimatedPaintMaterial> new_AnimatedPaintMaterial(std::shared_ptr<BitmapData> &param1, std::shared_ptr<BitmapData> &param2, std::shared_ptr<BitmapData> &param3, int numFramesX, int numFramesY, int fps, int numFrames, MipMapping mipMapping)
{
	auto p = std::make_shared<AnimatedPaintMaterial>(AnimatedPaintMaterial::TYPE,
													 param1,
													 param2,
													 param3,
													 numFramesX,
													 numFramesY,
													 fps,
													 numFrames,
													 mipMapping);
	p->ptr = p;
	return p;
}
