#include "bonusconst.h"
#include "mymath.h"

const double BonusConst::BONUS_HALF_SIZE = 75;
const double BonusConst::COS_ONE_DEGREE = MyMath::cos(MyMath::PI / 180.);
const double BonusConst::PARACHUTE_OFFSET_Z = 50;
const double BonusConst::BONUS_OFFSET_Z = 450;
const double BonusConst::ANGULAR_SPEED_Z = 0.1;
const double BonusConst::BOUND_SPHERE_RADIUS = (MyMath::sqrt(2.) * BonusConst::BONUS_HALF_SIZE) * 1.6;
