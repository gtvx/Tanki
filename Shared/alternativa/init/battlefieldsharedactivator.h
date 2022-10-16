#ifndef BATTLEFIELDSHAREDACTIVATOR_H
#define BATTLEFIELDSHAREDACTIVATOR_H

class OSGi;

class BattlefieldSharedActivator
{
public:
	BattlefieldSharedActivator();
	void start(OSGi *osgi);
	void stop(OSGi *osgi);
};


#endif // BATTLEFIELDSHAREDACTIVATOR_H
