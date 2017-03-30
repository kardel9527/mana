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

NMS_END // end namespace kcommon

#endif // __PETTY_H_

