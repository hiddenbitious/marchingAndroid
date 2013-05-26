#include "debug.h"
#include "globals.h"
#include <stdlib.h>	/// abort()

void handle_assert(const char *condition, const char *filename, const char *functionname, const int fileline)
{
	LOGE("%s: %d: %s: Assertion \'%s\' failed.\n", filename, fileline, functionname, condition);
	abort();
}
