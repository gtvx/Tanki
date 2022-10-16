#ifndef EFFECTSVISUALIZATIONMODEL_H
#define EFFECTSVISUALIZATIONMODEL_H

class ClientObject;
class Array;
class QString;
class ClientBattleEffect;
class TankModel;
class BattleEffect;
class ClientObjectUser;

#include "IEffectsVisualizationModel.h"

class EffectsVisualizationModel : public IEffectsVisualizationModel
{
	TankModel *tankInterface;
	QVector<BattleEffect*> *initialEffects;
public:
	EffectsVisualizationModel();
	virtual ~EffectsVisualizationModel();

	void initObject(ClientObject *clientObject, QVector<BattleEffect*> &effects);
	void effectActivated(ClientObjectUser *clientObject, const QString &userID, int itemIndex, int duration);
	void effectStopped(ClientObjectUser *clientObject, const QString &userID, int itemIndex);
	void objectLoaded(ClientObject *clientObject);
	void objectUnloaded(ClientObject *clientObject);
	QVector<ClientBattleEffect*> getInitialEffects(const QString &userId);
};

#endif // EFFECTSVISUALIZATIONMODEL_H
