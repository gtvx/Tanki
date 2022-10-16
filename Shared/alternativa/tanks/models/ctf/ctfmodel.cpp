#include "ctfmodel.h"
#include "flagdata.h"
#include "alternativa/object/clientobjectctf.h"
#include "initparams.h"
#include "service.h"
#include "alternativa/tanks/models/battlefield/battlefieldmodel.h"
#include "alternativa/tanks/models/battlefield/battlefielddata.h"
#include "ClientFlag.h"
#include "FlagsState.h"
#include "ctfflag.h"
#include "alternativa/engine3d/core/object3d.h"
#include "resource/resourceutil.h"
#include "battlecontroller.h"
#include "alternativa/tanks/models/tank/tankdata.h"
#include "alternativa/engine3d/objects/mesh.h"
#include "resource/tanks/tankresource.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/engine3d/objects/bsp.h"
#include "alternativa/physics/collisionprimitivelistitem.h"
#include "alternativa/physics/collisionprimitivelist.h"
#include "alternativa/physics/body.h"
#include "alternativa/tanks/physics/tankscollisiondetector.h"
#include "networking/network.h"
#include "alternativa/tanks/vehicles/tanks/tank.h"
#include "com/alternativaplatform/projects/tanks/client/models/tank/TankSpawnState.h"
#include "double_to_string.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "alternativa/object/clientobjectuser.h"
#include "alternativa/engine3d/core/object3d.h"
#include "gametime.h"
#include "resource/images/imageresource.h"
#include "resource/images/objectbitmapdata.h"
#include "alternativa/tanks/engine3d/MaterialType.h"
#include "alternativa/tanks/engine3d/materialsequence.h"
#include "alternativa/tanks/models/battlefield/battleview3d.h"
#include "alternativa/tanks/services/materialregistry/IMaterialRegistry.h"
#include "alternativa/tanks/engine3d/IMaterialSequenceRegistry.h"
#include "alternativa/tanks/engine3d/ITextureMaterialRegistry.h"
#include "alternativa/resource/stubbitmapdata.h"
#include "flash/display/bitmapdata.h"
#include "alternativa/engine3d/materials/texturematerial.h"
#include "alternativa/tanks/models/battlefield/scene3dcontainer/Scene3DContainer.h"
#include "_global.h"
#include <QHash>


static const int FLAG_LOCK_DURATION = 5000;
static const int FLAG_FRAME_WIDTH = 85;
static const int FLAG_FRAME_HEIGHT = 176;
static const double FLAG_MIPMAP_RESOLUTION = 2.5;


CTFModel::CTFModel(BattleController *battleController)
{
	this->battleController = battleController;
	this->flagLockTime = 0;
	this->userTankData = nullptr;
	this->flag_red = nullptr;
	this->flag_blue = nullptr;
	this->dropCommandSent = false;
	this->pedestal_red = nullptr;
	this->pedestal_blue = nullptr;
}

CTFModel::~CTFModel()
{
	_function_name("CTFModel::~CTFModel()");

	if (flag_red != nullptr)
		delete flag_red;
	if (flag_blue != nullptr)
		delete flag_blue;

	pedestal_red = nullptr;
	pedestal_blue = nullptr;
}

CTFFlag* CTFModel::getFlag(BattleTeamType flagTeam)
{
	if (flagTeam == BattleTeamType::RED)
		return flag_red;
	else
		return flag_blue;
}


void CTFModel::initObject(ClientObjectCTF *clientObject, const QString &blueFlagModelPedestalId, const QString &blueFlagTexturePedestalId, const QString &blueFlagTextureId, const QString &redFlagModelPedestalId, const QString &redFlagTexturePedestalId, const QString &redFlagTextureId, CaptureTheFlagSoundFX *sounds, Vector3 *posBlueFlag, Vector3 *posRedFlag)
{
	_function_name("CTFModel::initObject()");
	(void)sounds;
	this->materialRegistry = Service::getIMaterialRegistry();
	FlagData *redFlagData = new FlagData(redFlagTextureId, redFlagModelPedestalId, redFlagTexturePedestalId);
	FlagData *blueFlagData = new FlagData(blueFlagTextureId, blueFlagModelPedestalId, blueFlagTexturePedestalId);
	clientObject->initParams = new InitParams(redFlagData, blueFlagData);
	//this->flagDropSound = ResourceUtil.getResource(ResourceType.SOUND, "flagDropSound").sound;
	//this->flagReturnSound = ResourceUtil.getResource(ResourceType.SOUND, "flagReturnSound").sound;
	//this->flagTakeSound = ResourceUtil.getResource(ResourceType.SOUND, "flagTakeSound").sound;
	//this->winSound = ResourceUtil.getResource(ResourceType.SOUND, "winSound").sound;
	//this->initMessages();
	this->posBlueFlag.copy(posBlueFlag);
	this->posRedFlag.copy(posRedFlag);
	this->objectLoaded(nullptr);

	//var dumpService:IDumpService = IDumpService(Main.osgi.getService(IDumpService));
	//if (dumpService != null)
	//dumpService.registerDumper(this);
}

void CTFModel::initFlagsState(ClientObjectCTF *clientObject, FlagsState *state)
{
	_function_name("CTFModel::initFlagsState()");

	InitParams *initParams = clientObject->initParams;
	if (initParams == nullptr)
		throw 2423412;
	clientObject->initParams = nullptr;
	this->flag_blue = this->initFlag(BattleTeamType::BLUE, &state->blueFlag, initParams->blueFlagData);
	this->flag_red = this->initFlag(BattleTeamType::RED, &state->redFlag, initParams->redFlagData);
}


void CTFModel::objectLoaded(ClientObjectCTF*)
{
	_function_name("CTFModel::objectLoaded()");

	//var modelService:IModelService = IModelService(Main.osgi.getService(IModelService));
	//this->guiModel = (Main.osgi.getService(IBattlefieldGUI) as IBattlefieldGUI);
	this->battlefieldModel = Service::getBattlefieldModel();
	this->battlefieldModel->addPlugin(this);
	this->bfData = this->battlefieldModel->getBattlefieldData();
	this->tankModel = Service::getTankModel();
	//this->ctfMessages = this->battlefieldModel.messages;
	//Main.stage.addEventListener(KeyboardEvent.KEY_DOWN, this->onKey);
}

QString CTFModel::getBattlefieldPluginName()
{
	return "Capture The Flag";
}

void CTFModel::startBattle()
{

}

void CTFModel::restartBattle()
{

}

void CTFModel::finishBattle()
{

}

void CTFModel::tick(int time, int deltaMsec, double deltaSec, double interpolationCoeff)
{
	_function_name("CTFModel::tick()");

	(void)deltaMsec;
	(void)deltaSec;
	(void)interpolationCoeff;

	if (this->flag_blue == nullptr)
		return;

	if (((!(this->userTankData == nullptr)) && (this->userTankData->enabled)))
	{
		CTFFlag *userTeamFlag;
		CTFFlag *otherTeamFlag;

		if (this->userTankData->teamType == BattleTeamType::RED) {
			userTeamFlag = flag_red;
			otherTeamFlag = flag_blue;
		} else {
			userTeamFlag = flag_blue;
			otherTeamFlag = flag_red;
		}


		if (((!(otherTeamFlag->getState() == CTFFlagState::CARRIED)) && (time > this->flagLockTime)))
		{
			this->testFlagIntersection(otherTeamFlag);
		}
		else
		{
			if (otherTeamFlag->getCarrierData() == this->userTankData && userTeamFlag->getState() == CTFFlagState::AT_BASE)
			{
				this->testFlagIntersection(userTeamFlag);
			}
		}
		if (userTeamFlag->getState() == CTFFlagState::DROPPED)
		{
			this->testFlagIntersection(userTeamFlag);
		}
	}

	flag_red->update(deltaMsec);
	flag_blue->update(deltaMsec);
}

void CTFModel::addUser(ClientObjectUser *)
{

}

void CTFModel::removeUser(ClientObjectUser *user)
{
	_function_name("CTFModel::removeUser()");

	TankData *tankData = user->tankData;
	if (tankData == nullptr)
		return;

	if (flag_red->getCarrierData() == tankData)
		flag_red->returnToNone();

	if (flag_blue->getCarrierData() == tankData)
		flag_blue->returnToNone();
}

void CTFModel::addUserToField(ClientObjectUser *clientObject)
{
	_function_name("CTFModel::addUserToField()");

	TankData *tankData = this->tankModel->getTankData(clientObject);
	if (this->bfData->localUser == clientObject) {
		this->userTankData = tankData;
	}

	if (this->flag_blue != nullptr)
	{
		if (flag_red->getState() == CTFFlagState::CARRIED && flag_red->getCarrierId() == clientObject->getId()) {
			flag_red->setCarrier(flag_red->getCarrierId(), tankData);
		} else if (flag_blue->getState() == CTFFlagState::CARRIED && flag_blue->getCarrierId() == clientObject->getId()) {
			flag_blue->setCarrier(flag_blue->getCarrierId(), tankData);
		}
	}
}

void CTFModel::removeUserFromField(ClientObjectUser *)
{

}



CTFFlag* CTFModel::initFlag(BattleTeamType teamType, ClientFlag *flagSpec, FlagData *flagData)
{
	_function_name("CTFModel::initFlag()");

	std::shared_ptr<BitmapData> texture;

#ifdef GRAPHICS
	std::shared_ptr<BitmapData> imageResource = ResourceUtil::getResourceImage(flagData->imageResourceId)->bitmapData->data;
	if (imageResource != nullptr)
	{
		texture = imageResource;
	}
	else
	{
		texture = std::make_shared<BitmapData>(FLAG_FRAME_WIDTH, FLAG_FRAME_HEIGHT, false, (teamType == BattleTeamType::RED) ? 0xAA0000 : 170);
	}
#endif

	//IMaterialRegistry *materialRegistry = Service::getIMaterialRegistry();
	//MaterialSequence *materialSequence =
			//materialRegistry->materialSequenceRegistry()->getSequence(MaterialType::EFFECT, texture, FLAG_FRAME_WIDTH, FLAG_MIPMAP_RESOLUTION);


	Vector3 *position = &flagSpec->flagBasePosition;
	//materialSequence.materials
	//QVector<std::shared_ptr<Material>> *materials;
	CTFFlag *flag = new CTFFlag(teamType, position, FLAG_FRAME_WIDTH, FLAG_FRAME_HEIGHT, nullptr, texture, this->bfData->collisionDetector);

	flag->addToContainer(this->bfData->viewport->getMapContainer());

	if (flagSpec->flagCarrierId != nullptr)
	{
		TankData *carrierData = this->getTankData(flagSpec->flagCarrierId);
		flag->setCarrier(flagSpec->flagCarrierId, carrierData);
		//this->guiModel.ctfShowFlagCarried(teamType);
	}
	else
	{
		//if (flagSpec->flagPosition != nullptr)
		{
			position->copy(&flagSpec->flagPosition);
			if (!flag->isAtBase())
				flag->dropAt(position, this->bfData->collisionDetector);
			else
				flag->returnToBase();

			//this->guiModel->ctfShowFlagDropped(teamType);
		}
	}



	TankResource *tankResource = ResourceUtil::getResourceModel(flagData->pedestalModelId);
	if (tankResource == nullptr)
		throw 14134;


	std::shared_ptr<BSP> pedestal = new_BSP();
	pedestal->createTree(tankResource->mesh);

#ifdef GRAPHICS
	std::shared_ptr<BitmapData> texture_2 = ResourceUtil::getResourceImage(flagData->pedestalModel_img)->bitmapData->data;
	if (texture_2 == nullptr)
		texture_2 = std::make_shared<StubBitmapData>(0xFFFF00);

	double resolution = pedestal->calculateResolution(300, 300);

	TextureMaterial *material = materialRegistry->textureMaterialRegistry()->getMaterial(MaterialType::EFFECT, texture_2, resolution, false);
	pedestal->setMaterialToAllFaces(material);
#endif


	//createPedestal(this->materialRegistry,
	//flagData->pedestalModelId,
	//flagData->pedestalModel_img);

	Vector3 pos;
	pos.copy(flag->getBasePosition());
	pos.z -= CTFFlag::Z_DISPLACEMENT - 1;
	pedestal->position.copy(&pos);
	pedestal->matrix.setMatrix(&pedestal->position, &pedestal->rotation);

#ifndef GRAPHICS
	pedestal->bound.MinX = -100;
	pedestal->bound.MinY = -100;
	pedestal->bound.MinZ = -50;
	pedestal->bound.MaxX = 100;
	pedestal->bound.MaxY = 100;
	pedestal->bound.MaxZ = 50;
#endif

	if (teamType == BattleTeamType::RED)
		pedestal_red = pedestal;
	else
		pedestal_blue = pedestal;

#ifdef GRAPHICS
	this->bfData->viewport->getMapContainer()->addChild(pedestal.get());
#endif


	return flag;
}





TankData* CTFModel::getTankData(const QString &userId)
{
	_function_name("CTFModel::getTankData()");
	ClientObjectUser *userObject = battleController->tanks->value(userId);
	if (userObject == nullptr)
		return nullptr;
	TankData *tankData = this->tankModel->getTankData(userObject);
	if (tankData == nullptr || tankData->tank == nullptr)
		return nullptr;
	return tankData;
}



void CTFModel::testFlagIntersection(CTFFlag *flag)
{
	_function_name("CTFModel::testFlagIntersection()");

	if (((((flag->takeCommandSent) || (this->userTankData == nullptr)) || (this->userTankData->spawnState == TankSpawnState::NEWCOME)) || (this->userTankData->spawnState == TankSpawnState::SUICIDE)))
		return;


	Body *body = this->userTankData->tank;
	CollisionPrimitiveListItem *item = body->collisionPrimitives->head;
	while (item != nullptr)
	{
		CollisionPrimitive *primitive = item->primitive;
		if (this->bfData->collisionDetector->testCollision(primitive, (CollisionPrimitive*)flag->triggerCollisionPrimitive))
		{
			flag->takeCommandSent = true;
			this->attemptToTakeFlagCommand(this->bfData->bfObject, flag->teamType);
			return;
		}
		item = item->next;
	}
}



void CTFModel::attemptToTakeFlagCommand(ClientObject *bfObject, BattleTeamType flagTeamType)
{
	_function_name("CTFModel::attemptToTakeFlagCommand()");

	(void)bfObject;
	const Vector3 *tankPos = &this->userTankData->tank->state.pos;

	Service::getNetworker()->send("battle;attempt_to_take_flag;" +
								  BattleTeamType_toString(flagTeamType) +
								  QChar(';') + double_to_string(tankPos->x) +
								  QChar(';') + double_to_string(tankPos->y) +
								  QChar(';') + double_to_string(tankPos->z));
}


void CTFModel::flagTaken(ClientObject *clientObject, const QString &tankId, BattleTeamType flagTeam)
{
	_function_name("CTFModel::flagTaken()");

	(void)clientObject;

	if (this->flag_red == nullptr)
		return;

	CTFFlag *flag;

	if (flagTeam == BattleTeamType::RED)
		flag = flag_red;
	else
		flag = flag_blue;

	TankData *carrierData = this->getTankData(tankId);
	flag->setCarrier(tankId, carrierData);


	/*
	if (this->guiModel != nullptr)
	{
		if (this->userTankData != nullptr)
		{
			FlagMessage flagMessage = this->getFlagMessage(this->MESSAGE_TAKEN, (!(this->userTankData.teamType == flagTeam)));
			if (carrierData != nullptr)
			{
				this->ctfMessages->addMessage(flagMessage.color, ((carrierData.userName + " ") + flagMessage.text));
			}
			this->guiModel.logUserAction(tankId, flagMessage.text);
		}
		else
		{
			if (this->battlefieldModel.spectatorMode)
			{
				if (carrierData != null)
				{
					QString msg = this->locale.getText(TextConst.CTF_GOT_FLAG);
					switch (carrierData.teamType)
					{
						case BattleTeamType.RED:
							this->ctfMessages.addMessage(COLOR_RED, ((carrierData.userName + " ") + msg));
							break;
						case BattleTeamType.BLUE:
							this->ctfMessages.addMessage(COLOR_BLUE, ((carrierData.userName + " ") + msg));
							break;
						case BattleTeamType.NONE:
							this->ctfMessages.addMessage(COLOR_NEGATIVE, ((carrierData.userName + " ") + msg));
							break;
					}
					this->guiModel.logUserAction(tankId, msg);
				}
			}
		}
		this->battlefieldModel.soundManager.playSound(this->flagTakeSound, 0, 1);
	}
	*/

	//this->guiModel.ctfShowFlagCarried(flagTeam);
	flag->takeCommandSent = false;
	//CTFFlag(this->flags[((flagTeam == BattleTeamType.RED) ? BattleTeamType.BLUE : BattleTeamType.RED)]).takeCommandSent = false;

	if (flagTeam == BattleTeamType::RED)
		flag_blue->takeCommandSent = false;
	else
		flag_red->takeCommandSent = false;

}



void CTFModel::returnFlagToBase(ClientObject *clientObject, BattleTeamType flagTeam, const QString &tankId)
{
	_function_name("CTFModel::returnFlagToBase()");

	(void)clientObject;
	(void)tankId;

	this->returnFlag(flagTeam);

	/*
	if (this->userTankData != null)
	{
		flagMessage = this->getFlagMessage(this->MESSAGE_RETURNED, (this->userTankData.teamType == flagTeam));
		if (((tankId == null) || (tankId == "null")))
		{
			msg = ((this->userTankData.teamType == flagTeam) ? this->ourFlagReturnedMessage : this->enemyFlagReturnedMessage);
			this->ctfMessages.addMessage(flagMessage.color, msg);
			this->guiModel.logAction(msg);
		}
		else
		{
			userData = this->getTankData(tankId, null);
			if (userData != null)
			{
				this->ctfMessages.addMessage(flagMessage.color, ((userData.userName + " ") + flagMessage.text));
				this->guiModel.logUserAction(tankId, flagMessage.text);
			}
		}
	}
	else
	{
		if (this->battlefieldModel.spectatorMode)
		{
			if (((tankId == null) || (tankId == "null")))
			{
				if (flagTeam == BattleTeamType.RED)
				{
					msg = this->locale.getText(TextConst.CTF_RED_FLAG_RETURNED);
					this->ctfMessages.addMessage(COLOR_RED, msg);
				}
				else
				{
					msg = this->locale.getText(TextConst.CTF_BLUE_FLAG_RETURNED);
					this->ctfMessages.addMessage(COLOR_BLUE, msg);
				}
				this->guiModel.logAction(msg);
			}
			else
			{
				userData = this->getTankData(tankId, null);
				if (userData != null)
				{
					msg = this->locale.getText(TextConst.CTF_RETURNED_FLAG);
					switch (userData.teamType)
					{
						case BattleTeamType.RED:
							this->ctfMessages.addMessage(COLOR_RED, ((userData.userName + " ") + msg));
							break;
						case BattleTeamType.BLUE:
							this->ctfMessages.addMessage(COLOR_BLUE, ((userData.userName + " ") + msg));
							break;
						case BattleTeamType.NONE:
							this->ctfMessages.addMessage(COLOR_NEGATIVE, ((userData.userName + " ") + msg));
							break;
					}
					this->guiModel.logUserAction(tankId, msg);
				}
			}
		}
	}
	this->battlefieldModel.soundManager.playSound(this->flagReturnSound, 0, 1);
	*/
}

void CTFModel::dropFlag(ClientObject *clientObject, Vector3 *position, BattleTeamType flagTeam)
{
	_function_name("CTFModel::dropFlag()");

	(void)clientObject;

	if (this->flag_red == nullptr)
		return;

	CTFFlag *flag = this->getFlag(flagTeam);

	if (((flag->getCarrierData() == this->userTankData) && this->dropCommandSent))
	{
		this->dropCommandSent = false;
		this->flagLockTime = GetGameTime() + FLAG_LOCK_DURATION;
	}


	/*
	if (((!(this->guiModel == null)) && (!(flag.carrierData == null))))
	{
		if (this->userTankData != null)
		{
			flagMessage = this->getFlagMessage(this->MESSAGE_LOST, (this->userTankData.teamType == flagTeam));
			this->ctfMessages.addMessage(flagMessage.color, ((flag.carrierData.userName + " ") + flagMessage.text));
			this->guiModel.logUserAction(flag.carrierId, flagMessage.text);
		}
		else
		{
			if (this->battlefieldModel.spectatorMode)
			{
				msg = this->locale.getText(TextConst.CTF_LOST_FLAG);
				switch (flag.carrierData.teamType)
				{
					case BattleTeamType.RED:
						this->ctfMessages.addMessage(COLOR_RED, ((flag.carrierData.userName + " ") + msg));
						break;
					case BattleTeamType.BLUE:
						this->ctfMessages.addMessage(COLOR_BLUE, ((flag.carrierData.userName + " ") + msg));
						break;
					case BattleTeamType.NONE:
						this->ctfMessages.addMessage(COLOR_NEGATIVE, ((flag.carrierData.userName + " ") + msg));
						break;
				}
				this->guiModel.logUserAction(flag.carrierData.userName, msg);
			}
		}
		this->battlefieldModel.soundManager.playSound(this->flagDropSound, 0, 1);
		if (((flag.carrierData == this->userTankData) && (this->dropCommandSent)))
		{
			this->dropCommandSent = false;
			this->flagLockTime.value = (getTimer() + FLAG_LOCK_DURATION.value);
		}
	}
	*/

	flag->dropAt(position, this->bfData->collisionDetector);
	//this->guiModel.ctfShowFlagDropped(flagTeam);
}

void CTFModel::flagDelivered(ClientObject *clientObject, BattleTeamType winnerTeam, const QString &delivererTankId)
{
	_function_name("CTFModel::flagDelivered()");

	(void)clientObject;
	(void)delivererTankId;

	if (this->flag_red == nullptr)
		return;
	BattleTeamType otherTeam = winnerTeam == BattleTeamType::RED ? BattleTeamType::BLUE : BattleTeamType::RED;
	this->returnFlag(otherTeam);
	/*
	TankData *delivererData = this->getTankData(delivererTankId, nullptr);

	if (delivererData != nullptr)
	{
		if (this->userTankData != null)
		{
			flagMessage = this->getFlagMessage(this->MESSAGE_CAPTURED, (this->userTankData.teamType == winnerTeam));
			this->ctfMessages.addMessage(flagMessage.color, ((delivererData.userName + " ") + flagMessage.text));
			this->guiModel.logUserAction(delivererTankId, flagMessage.text);
		}
		else
		{
			if (this->battlefieldModel.spectatorMode)
			{
				msg = this->locale.getText(TextConst.CTF_CAPTURED_FLAG);
				switch (delivererData.teamType)
				{
					case BattleTeamType.RED:
						this->ctfMessages.addMessage(COLOR_RED, ((delivererData.userName + " ") + msg));
						break;
					case BattleTeamType.BLUE:
						this->ctfMessages.addMessage(COLOR_BLUE, ((delivererData.userName + " ") + msg));
						break;
					case BattleTeamType.NONE:
						this->ctfMessages.addMessage(COLOR_NEGATIVE, ((delivererData.userName + " ") + msg));
						break;
				}
				this->guiModel.logUserAction(delivererTankId, msg);
			}
		}
		this->battlefieldModel.soundManager.playSound(this->winSound, 0, 1);
	}
	*/
}



void CTFModel::returnFlag(BattleTeamType flagTeam)
{
	_function_name("CTFModel::returnFlag()");

	if (this->flag_blue == nullptr)
		return;
	CTFFlag *flag = getFlag(flagTeam);
	flag->returnToBase();
	flag = getFlag(flag->teamType == BattleTeamType::RED ? BattleTeamType::BLUE : BattleTeamType::RED);
	flag->takeCommandSent = false;
	//this->guiModel.ctfShowFlagAtBase(flagTeam);
}


void CTFModel::doDropFlag()
{
	_function_name("CTFModel::doDropFlag()");

	if ((((((this->flag_red == nullptr) || (this->bfData == nullptr)) || (this->userTankData == nullptr)) || (this->dropCommandSent)) || (!(this->tankModel->getUserControlsEnabled()))))
		return;

	CTFFlag *flag = this->getFlag(this->userTankData->teamType == BattleTeamType::BLUE ? BattleTeamType::RED : BattleTeamType::BLUE);
	if (((flag == nullptr) || (!(flag->getCarrierData() == this->userTankData))))
		return;

	this->dropCommandSent = true;
	this->dropFlagCommand(this->bfData->bfObject, &this->userTankData->tank->state.pos);
}

void CTFModel::dropFlagCommand(ClientObject *bfObject, Vector3 *pos)
{
	_function_name("CTFModel::dropFlagCommand()");

	(void)bfObject;

	QString str;
	str.append("battle;flag_drop;{\"y\":");
	str.append(double_to_string(pos->y));
	str.append(",\"z\":");
	str.append(double_to_string(pos->z));
	str.append(",\"x\":");
	str.append(double_to_string(pos->x));
	str.append("}");
	Service::getNetworker()->send(str);
}

void CTFModel::objectUnloaded(ClientObject*)
{
	_function_name("CTFModel::objectUnloaded()");

	flag_red->dispose();
	flag_blue->dispose();

	//Main.stage.removeEventListener(KeyboardEvent.KEY_DOWN, this.onKey);

	this->battlefieldModel->removePlugin(this);
	//this->guiModel = nullptr;
	this->battlefieldModel = nullptr;
	this->bfData = nullptr;
	this->userTankData = nullptr;
}
