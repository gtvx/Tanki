#ifndef CORDS_H
#define CORDS_H


#include "__bonus.h"

#if __BONUS_TYPE == 1
#include "cords1.h"
typedef Cords1 Cords;
#endif

#if __BONUS_TYPE == 2
#include "cords2.h"
typedef Cords2 Cords;
#endif

#endif // CORDS_H
