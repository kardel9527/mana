#ifndef __K_MACROS_H_
#define __K_MACROS_H_

#define NMS_BEGIN(NAME) namespace NAME {
#define NMS_END }

#define sfree(p) do { if (p) free(p); p = 0; } while(0)
#define sdelete(p) do { if (p) delete(p); p = 0; } while(0)

template<typename T>
inline T min(const T &l, const T &r) { return l > r ? r : l; }

template<typename T>
inline T max(const T &l, const T &r) { return l < r ? r :l; }

#endif //__K_MACROS_H_