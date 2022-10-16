#include "garagemodelactivator.h"
#include "alternativa/tanks/model/garagemodel.h"


OSGi *GarageModelActivator::osgi;


GarageModelActivator::GarageModelActivator()
{
	this->garageModel = nullptr;
	this->itemModel = nullptr;
	this->item3DModel = nullptr;
	this->itemEffectModel = nullptr;
}

void GarageModelActivator::start(OSGi *osgi)
{
	GarageModelActivator::osgi = osgi;
	//IModelService *modelRegister = (osgi.getService(IModelService) as IModelService);
	this->garageModel = new GarageModel();
	//modelRegister->add(this->garageModel);
	//this->itemModel = new ItemModel();
	//modelRegister->add(this->itemModel);
	//this->item3DModel = new Item3DModel();
	//modelRegister->add(this->item3DModel);
	//this->itemEffectModel = new ItemEffectModel();
	//modelRegister->add(this->itemEffectModel);
}

void GarageModelActivator::stop(OSGi *osgi)
{
	(void)osgi;
	//IModelService *modelRegister = (osgi.getService(IModelService) as IModelService);
	//modelRegister->remove(this->garageModel->id);
	//modelRegister->remove(this->itemModel->id);
	//modelRegister->remove(this->itemEffectModel->id);
	this->garageModel = nullptr;
	this->itemModel = nullptr;
	this->item3DModel = nullptr;
	this->itemEffectModel = nullptr;
	GarageModelActivator::osgi = nullptr;
}
