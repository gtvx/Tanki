#if 0
#include "ctfflagstate.h"
#include <QString>

/*
class QString;

class CTFFlagState
{
	int _value;
	QString *_stringValue;
public:
	CTFFlagState();

	static const CTFFlagState *AT_BASE;
	static const CTFFlagState *DROPPED;
	static const CTFFlagState *CARRIED;

	CTFFlagState(int value, const QString &stringValue);
	int getValue();
	QString toString();
};
*/


const CTFFlagState *CTFFlagState::AT_BASE = new CTFFlagState(0, "AT_BASE");
const CTFFlagState *CTFFlagState::DROPPED = new CTFFlagState(1, "DROPPED");
const CTFFlagState *CTFFlagState::CARRIED = new CTFFlagState(2, "CARRIED");



CTFFlagState::CTFFlagState(int value, const QString &stringValue)
{
	this->_value = value;
	this->_stringValue = new QString(stringValue);
}

int CTFFlagState::getValue()
{
	return this->_value;
}

QString CTFFlagState::toString()
{
	return "CTFFlagState [" + *this->_stringValue + "]";
}
#endif
