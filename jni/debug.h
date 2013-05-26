#ifndef _DEBUG_H_
#define _DEBUG_H_

#define c_assert(condition)		if(!(condition)) { handle_assert(#condition, __FILE__, __FUNCTION__, __LINE__); }

void handle_assert(const char *condition, const char *filename, const char *functionname, const int fileline);

#endif
