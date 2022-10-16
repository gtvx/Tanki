#ifndef PARABONUS_H
#define PARABONUS_H

#include "__bonus.h"

#if __BONUS_TYPE == 1
#include "parabonus1.h"
typedef ParaBonus1 ParaBonus;
#endif

#if __BONUS_TYPE == 2
#include "parabonus2.h"
typedef ParaBonus2 ParaBonus;
#endif

#endif // PARABONUS_H
