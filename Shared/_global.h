#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "__global.h"

extern char const * __function_name;
#define _function_name(name) __function_name = name;

#endif // _GLOBAL_H
