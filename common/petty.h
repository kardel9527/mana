#ifndef __PETTY_H_
#define __PETTY_H_
#include "macros.h"
#include "udt.h"

NMS_BEGIN(kcommon)

template<typename T>
inline T min(const T &l, const T &r) { return l > r ? r : l; }

template<typename T>
inline T max(const T &l, const T &r) { return l < r ? r :l; }

template<typename T>
inline void swap(T &t1, T &t2) { t1 += t2; t2 = t1 - t2; t1 = t1 - t2; }

inline uint32 roundup_pow_of_two(uint32 x) { if (!(x & (x - 1)) && x > 1) return x; int p = 0; while (x != 0) { x >>= 1; ++p; } return 1u << p; }

inline uint32 pow_of_two_align(uint32 x) { bool aligned = !(x & (x - 1)) && x > 1; int p = 0; while (x != 0) { x >>= 1; ++p; } return  aligned ? p - 1 : p; }

NMS_END // end namespace kcommon

#endif // __PETTY_H_

