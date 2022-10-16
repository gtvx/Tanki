#include "effectsvisualizationmodel.h"
#include "projects/tanks/client/battlefield/gui/models/effectsvisualization/battleeffect.h"
#include "flash/utils/gettimer.h"
#include "alternativa/tanks/models/tank/tankmodel.h"
#include "service.h"
#include "clientbattleeffect.h"


EffectsVisualizationModel::EffectsVisualizationModel()
{
	//_interfaces.push(IModel, IEffectsVisualizationModel, IObjectLoadListener);
	//Main.osgi.registerService(IEffectsVisualizationModel, this);
}

EffectsVisualizationModel::~EffectsVisualizationModel()
{

}

void EffectsVisualizationModel::initObject(ClientObject *clientObject, QVector<BattleEffect*> &effects)
{
	(void)clientObject;

	/*
	var modelService:IModelService;
	if (effects == null)
	{
		this->initialEffects = null;
	}
	else
	{
		this->initialEffects = Vector.<BattleEffect>(effects);
	}
	if (this->tankInterface == null)
	{
		modelService = IModelService(Main.osgi.getService(IModelService));
		this->tankInterface = TankModel(Main.osgi.getService(ITank));
	}
	*/

	if (effects.isEmpty())
	{
		this->initialEffects = nullptr;
	}
	else
	{
		this->initialEffects = new QVector<BattleEffect*>(effects);
	}

	this->tankInterface = (TankModel*)Service::getITank();
}

void EffectsVisualizationModel::effectActivated(ClientObjectUser *clientObject, const QString &userID, int itemIndex, int duration)
{
	(void)userID;
	ClientObjectUser *userObject = clientObject;
	if (userObject != nullptr)
	{
		this->tankInterface->effectStarted(userObject, itemIndex, duration);
	}
}

void EffectsVisualizationModel::effectStopped(ClientObjectUser *clientObject, const QString &userID, int itemIndex)
{
	if (this->initialEffects != nullptr)
	{
		int len = this->initialEffects->length();
		for (int i = 0; i < len; i++)
		{
			BattleEffect *effect = this->initialEffects->at(i);
			if (((effect->userID == userID) && (effect->itemIndex == itemIndex)))
			{
				this->initialEffects->removeAt(i);
				return;
			}
		}
	}

	ClientObjectUser *userObject = clientObject;
	if (userObject != nullptr)
	{
		this->tankInterface->effectStopped(userObject, itemIndex);
	}
}

void EffectsVisualizationModel::objectLoaded(ClientObject *clientObject)
{
	(void)clientObject;
}

void EffectsVisualizationModel::objectUnloaded(ClientObject *clientObject)
{
	(void)clientObject;
	this->initialEffects = nullptr;
}

QVector<ClientBattleEffect*> EffectsVisualizationModel::getInitialEffects(const QString &userId)
{
	QVector<ClientBattleEffect*> result;

	if (this->initialEffects == nullptr)
		return QVector<ClientBattleEffect*>();


	int now = getTimer();
	for (int i = 0; i < this->initialEffects->length(); i++)
	{
		BattleEffect *effect = this->initialEffects->at(i);
		if (effect->userID == userId)
		{
			result.append(new ClientBattleEffect(now, effect->userID, effect->itemIndex, effect->durationTime));
			//this->initialEffects->splice(i, 1);
			this->initialEffects->removeAt(i);
			i--;
		}
	}

	if (this->initialEffects->length() == 0)
	{
		this->initialEffects = nullptr;
	}

	return result;
}
