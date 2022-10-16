#ifndef MATERIALSEQUENCEREGISTRY_H
#define MATERIALSEQUENCEREGISTRY_H

#include "IMaterialSequenceRegistry.h"
#include <QVector>
#include <QHash>

class QTimer;
class QObject;
enum class MaterialType;


class MaterialSequenceRegistry : public IMaterialSequenceRegistry
{
	bool _useMipMapping;
	QHash<BitmapData*, MaterialSequence*> sequenceByImage;
	QVector<MaterialSequence*> scheduledSequences;
	int numScheduledSequences;
	QTimer *timer;
	QObject *o;
public:
	MaterialSequenceRegistry(int timerInterval);
	~MaterialSequenceRegistry();


	void setTimerInterval(int value) override;

	bool getUseMipMapping() override;

	void setUseMipMapping(bool value) override;

	MaterialSequence* getSequence(MaterialType materialType,
								  BitmapData *sourceImage,
								  int frameWidth,
								  double mipMapResolution,
								  bool disposeSourceImage = false,
								  bool createMirroredFrames = false);


	MaterialSequence* getSquareSequence(MaterialType materialType,
										BitmapData *sourceImage,
										double mipMapResolution,
										bool disposeSourceImage = false,
										bool createMirroredFrames = false);

	void clear() override;

	QString getDump() override;

	void disposeSequence(MaterialSequence *sequence) override;

	void scheduleSequence(MaterialSequence *sequence);


private:
	void onTimer();
	void removeScheduledSequence(int index);
};

#endif // MATERIALSEQUENCEREGISTRY_H
