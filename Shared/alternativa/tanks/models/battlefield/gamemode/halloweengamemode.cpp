#include "halloweengamemode.h"
#include "../battleview3d.h"
#include "alternativa/engine3d/lights/directionallight.h"
#include "alternativa/tanks/camera/gamecamera.h"
#include "alternativa/math/matrix3.h"
#include "alternativa/engine3d/core/shadowmap.h"
#include "alternativa/tanks/model/panel/IBattleSettings.h"



void HalloweenGameMode::applyChanges(BattleView3D *viewport)
{
	GameCamera *camera = viewport->camera;
	this->camera = camera;
	light = new_DirectionalLight(1315346);
	camera->directionalLight = light.get();
	Vector3 direction(1, 1, 1);
	Matrix3 matrix;
	matrix.setRotationMatrix(-2.6, 0.16, -2.8);
	Vector3 vector(0, 1, 0);
	vector.transformBy3(&matrix);
	direction.x = vector.x;
	direction.y = vector.y;
	direction.z = vector.z;
	camera->directionalLight->lookAt(direction.x, direction.y, direction.z);
	camera->ambientColor = 602185;
	camera->shadowMap = new ShadowMap(0x0800, 7500, 15000, 1, 10000);
	camera->directionalLightStrength = 1;
	camera->shadowMapStrength = 1.5;
	camera->shadowMap->bias = 1;
	camera->useShadowMap = true;
	camera->deferredLighting = true;
	camera->fogColor = 8880256;
	camera->fogNear = 5000;
	camera->fogFar = 10000;
	camera->fogAlpha = 0.25;
	camera->ssao = true;
	camera->ssaoRadius = 400;
	camera->ssaoRange = 450;
	camera->ssaoColor = 2636880;
	camera->ssaoAlpha = 1.5;
}

void HalloweenGameMode::applyChangesBeforeSettings(IBattleSettings *settings)
{
	if (settings->fog() && this->camera->fogStrength != 1)
	{
		this->camera->fogStrength = 1;
		this->camera->fogColor = 8880256;
		this->camera->fogNear = 5000;
		this->camera->fogFar = 10000;
		this->camera->fogAlpha = 0.25;
	}
	else
	{
		if (!settings->fog())
		{
			this->camera->fogStrength = 0;
		}
	}
	if (settings->shadows() && !this->camera->useShadowMap)
	{
		this->camera->useShadowMap = true;
		if (this->camera->directionalLight != nullptr)
		{
			this->camera->directionalLight->useShadowMap = true;
		}
		this->camera->shadowMapStrength = 1;
	}
	else
	{
		if (!settings->shadows())
		{
			this->camera->useShadowMap = false;
			if (this->camera->directionalLight != nullptr)
			{
				this->camera->directionalLight->useShadowMap = false;
			}
			this->camera->shadowMapStrength = 0;
		}
	}
	if (settings->defferedLighting() && this->camera->directionalLightStrength != 1)
	{
		this->camera->directionalLight->intensity = 1;
		this->camera->directionalLightStrength = 1;
		this->camera->deferredLighting = true;
		this->camera->deferredLightingStrength = 1;
	}
	else
	{
		if (!settings->defferedLighting())
		{
			this->camera->directionalLight->intensity = 0;
			this->camera->directionalLightStrength = 0;
			this->camera->deferredLighting = false;
			this->camera->deferredLightingStrength = 0;
		}
	}

	this->camera->ssao = settings->useSSAO();
}

BitmapData* HalloweenGameMode::applyColorchangesToSkybox(BitmapData *skybox)
{
	return skybox;
}
