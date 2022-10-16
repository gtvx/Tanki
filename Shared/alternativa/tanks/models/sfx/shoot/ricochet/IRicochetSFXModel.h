#ifndef IRICOCHETSFXMODEL_H
#define IRICOCHETSFXMODEL_H

class RicochetSFXData;
class ClientObjectWeapon;

class IRicochetSFXModel
{
public:
	 virtual RicochetSFXData* getSfxData(ClientObjectWeapon*) = 0;
};

#endif // IRICOCHETSFXMODEL_H
