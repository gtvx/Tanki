#ifndef CLIENTBATTLEEFFECT_H
#define CLIENTBATTLEEFFECT_H

#include <QString>

class ClientBattleEffect
{
public:
	int receiveTime;
	QString userId;
	int effectId;
	int duration;

	ClientBattleEffect(int receiveTime, const QString &userId, int effectId, int duration)
	{
		this->receiveTime = receiveTime;
		this->userId = userId;
		this->effectId = effectId;
		this->duration = duration;
	}
};

#endif // CLIENTBATTLEEFFECT_H
