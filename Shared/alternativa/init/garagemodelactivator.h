#ifndef GARAGEMODELACTIVATOR_H
#define GARAGEMODELACTIVATOR_H

class OSGi;
class GarageModel;
class ItemModel;
class Item3DModel;
class ItemEffectModel;


class GarageModelActivator
{	
public:	
	static OSGi *osgi;
	GarageModel *garageModel;
	ItemModel *itemModel;
	Item3DModel *item3DModel;
	ItemEffectModel *itemEffectModel;

	GarageModelActivator();

	void start(OSGi *osgi);

	void stop(OSGi *osgi);
};

#endif // GARAGEMODELACTIVATOR_H
