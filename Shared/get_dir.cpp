#include "get_dir.h"
#include "__global.h"
#include <QString>

QString get_dir()
{
#ifdef L_MT
	return "files/MT";
#elif L_GT
	return "files/GT";
#elif L_OT
	return "files/OT";
#elif L_PRO
	return "files/PRO";
#endif
}
