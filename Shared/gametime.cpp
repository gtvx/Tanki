#include "gametime.h"

static int32_t time;

void SetGameTime(int32_t time)
{
	::time = time;
}

int32_t GetGameTime()
{
	return ::time;
}
