#ifndef COMMON_H__
#define COMMON_H__

inline int verifyNonNull(void *ptr)
{

return __builtin_expect (ptr != NULL, 1);
}

#endif
