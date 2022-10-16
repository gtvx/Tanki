#ifndef FLAGDATA_H
#define FLAGDATA_H

#include <QString>

class FlagData
{
public:
	QString imageResourceId;
	QString pedestalModelId;
	QString pedestalModel_img;

	FlagData(const QString &imageResourceId, const QString &pedestalModelId, const QString &pedestalModel_img);
};

#endif // FLAGDATA_H
